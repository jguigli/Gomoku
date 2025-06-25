#include "../../includes/game.hpp"


Game::Game() : _blackTurn(true), _blackStone(CELL_BLACK), _whiteStone(CELL_WHITE), _agent(nullptr), _agentActive(false), _gameMode(PLAYER_VS_PLAYER), _difficulty(NORMAL), _backToMenu(false)
{
    _resetButton = { BOARD_WIDTH / 2 - 120, BOARD_HEIGHT - 25, 100, 40 };
    _menuButton = { BOARD_WIDTH / 2 + 20, BOARD_HEIGHT - 25, 100, 40 };
    _agent = new Agent(static_cast<int>(_difficulty), CELL_WHITE);
    std::cout<< "CELL_WHITE : " << CELL_WHITE << std::endl;
    _turnStartTime = std::chrono::high_resolution_clock::now();
}

Game::Game(GameMode gameMode, Difficulty difficulty) : _blackTurn(true), _blackStone(CELL_BLACK), _whiteStone(CELL_WHITE), _agent(nullptr), _agentActive(false), _gameMode(gameMode), _difficulty(difficulty), _backToMenu(false)
{
    _resetButton = { BOARD_WIDTH / 2 - 120, BOARD_HEIGHT - 25, 100, 40 };
    _menuButton = { BOARD_WIDTH - 150, BOARD_HEIGHT - 25, 100, 40 };
    _agent = new Agent(static_cast<int>(_difficulty), CELL_WHITE);
    _agentActive = (gameMode == PLAYER_VS_AI);
    std::cout<< "Game mode: " << (_gameMode == PLAYER_VS_AI ? "Player vs AI" : "Player vs Player") << std::endl;
    std::cout<< "Difficulty level: " << static_cast<int>(_difficulty) << std::endl;
    _turnStartTime = std::chrono::high_resolution_clock::now();
}

Game::~Game() {
    delete _agent;
}

void Game::setGameMode(GameMode gameMode, Difficulty difficulty) {
    _gameMode = gameMode;
    _difficulty = difficulty;
    _agentActive = (gameMode == PLAYER_VS_AI);
    
    // Update agent with new difficulty
    delete _agent;
    _agent = new Agent(static_cast<int>(_difficulty), CELL_WHITE);
    
    reset(); // Reset the game state
}

void Game::reset()
{
    _board.reset();
    _blackTurn = true;
    _blackStone.resetPairCaptured();
    _whiteStone.resetPairCaptured();
    _turnStartTime = std::chrono::high_resolution_clock::now();
}

bool Game::checkFiveStonesAligned(int x, int y, Player stone)
{
    const int directions[4][2] = {
        {1, 0},   // horizontal
        {0, 1},   // vertical
        {1, 1},   // diagonal gauche droite
        {1, -1}   // diagonal droite gauche
    };

    for (auto& dir : directions)
    {
        int count = 1;

        // FORWARD
        for (int i = 1; i < 5; ++i)
        {
            int nx = x + dir[0] * i;
            int ny = y + dir[1] * i;
            if (_board.isInsideGrid(nx, ny) && _board.getCell(nx, ny) == stone.getStoneColor())
                count++;
            else
                break;
        }

        // BACKWARD
        for (int i = 1; i < 5; ++i)
        {
            int nx = x - dir[0] * i;
            int ny = y - dir[1] * i;
            if (_board.isInsideGrid(nx, ny) && _board.getCell(nx, ny) == stone.getStoneColor())
                count++;
            else
                break;
        }

        if (count >= 5)
            return true;
    }
    return false;
}

bool Game::checkCapturePair(int x, int y, Player stone)
{
    const int directions[4][2] = {
        {1, 0},   // horizontal
        {0, 1},   // vertical
        {1, 1},   // diagonal gauche droite
        {1, -1}   // diagonal droite gauche
    };

    Cell opponentStone = stone.getStoneColor() == CELL_BLACK ? CELL_WHITE : CELL_BLACK;

    for (auto& dir : directions)
    {
        int count = 0;

        // FORWARD
        for (int i = 1; i < 4; ++i)
        {
            int nx = x + dir[0] * i;
            int ny = y + dir[1] * i;
            if (_board.isInsideGrid(nx, ny) && _board.getCell(nx, ny) == opponentStone)
                count++;
            else if (_board.isInsideGrid(nx, ny) && _board.getCell(nx, ny) == stone.getStoneColor() && count == 2)
            {
                _board.emptyCell(x + dir[0] * (i - 1), y + dir[1] * (i - 1));
                _board.emptyCell(x + dir[0] * (i - 2), y + dir[1] * (i - 2));
                stone.addCapturedPair();
                return true;
            }
            else
                break;
        }

        // BACKWARD
        for (int i = 1; i < 5; ++i)
        {
            int nx = x - dir[0] * i;
            int ny = y - dir[1] * i;
            if (_board.isInsideGrid(nx, ny) && _board.getCell(nx, ny) == opponentStone)
                count++;
            else if (_board.isInsideGrid(nx, ny) && _board.getCell(nx, ny) == stone.getStoneColor() && count == 2)
            {
                _board.emptyCell(x - dir[0] * (i - 1), y - dir[1] * (i - 1));
                _board.emptyCell(x - dir[0] * (i - 2), y - dir[1] * (i - 2));
                stone.addCapturedPair();
                return true;
            }
            else
                break;
        }
    }
    return false;
}

bool Game::spotFreeThreePattern(int x, int y, int dx, int dy, Player stone)
{
	std::string pattern;
	const std::vector<std::string> validPatterns = {
		"_OOO_", "_O_OO_", "_OO_O_",
	};

	for (int i = -4; i <= 4; ++i)
	{
		int nx = x + i * dx;
		int ny = y + i * dy;

		if (!_board.isInsideGrid(nx, ny))
		{
			pattern += "X";
			continue;
		}

		if (nx == x && ny == y)
		{
			pattern += "O";
			continue;
		}

		Cell pos = _board.getCell(nx, ny);
		if (pos == CELL_EMPTY)
			pattern += "_";
		else if (pos == stone.getStoneColor())
			pattern += "O";
		else
			pattern += "X";
	}

	for (const auto& p : validPatterns)
	{
		if (pattern.find(p) != std::string::npos)
			return true;
	}

	return false;
}

bool Game::checkDoubleFreeThree(int x, int y, Player stone)
{
    const int directions[4][2] = {
        {1, 0},   // horizontal
        {0, 1},   // vertical
        {1, 1},   // diagonal gauche droite
        {1, -1}   // diagonal droite gauche
    };
    
	if (!_board.isInsideGrid(x, y))
		return false;

	int count = 0;

	for (auto& direction : directions)
	{
		if (spotFreeThreePattern(x, y, direction[0], direction[1], stone))
			count++;
	}

	return count >= 2;
}

bool Game::handlePlayerMove(int x, int y, Player currentPlayer)
{
    if (!checkDoubleFreeThree(x, y, currentPlayer) || 
        (checkDoubleFreeThree(x, y, currentPlayer) && checkCapturePair(x, y, currentPlayer)))
    {
        if (_board.placePiece(x, y, currentPlayer))
            return true;
    }
    return false;
}

void Game::update()
{

    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mouse = GetMousePosition();
        if (CheckCollisionPointRec(mouse, _resetButton))
        {
            reset();
        }
        else if (CheckCollisionPointRec(mouse, _menuButton))
        {
            _backToMenu = true;
        }
        else if (mouse.y < BOARD_HEIGHT)
        {
            int x = round((mouse.x - BOARD_MARGIN) / static_cast<float>(CELL_SIZE));
            int y = round((mouse.y - BOARD_MARGIN) / static_cast<float>(CELL_SIZE));
            
            Player currentPlayer = _blackTurn ? _blackStone : _whiteStone;
            if (handlePlayerMove(x, y, currentPlayer))
            {
                auto now = std::chrono::high_resolution_clock::now();
                _elapsedMs = std::chrono::duration<float, std::milli>(now - _turnStartTime).count();
                _turnStartTime = now;
                std::cout << "Human player took : " << (int)_elapsedMs << " ms to play" << std::endl;
                if (checkFiveStonesAligned(x, y, currentPlayer))
                {
                    std::cout << "GAME OVER" << std::endl;
                }
                
                checkCapturePair(x, y, currentPlayer);
                if (currentPlayer.getPairCaptured() >= 5)
                {
                    std::cout << "GAME OVER" << std::endl;
                }
                _blackTurn = !_blackTurn;
                _turnStartTime = std::chrono::high_resolution_clock::now();
            }
        }
    }

    if (_agentActive && !_blackTurn)
    {
        auto boardVec = _board.getGrid();
        auto result = _agent->play(boardVec);
        if (result.movePlayed)
        {
             _iaElapsedMs = result.elapsedMs;
            std::cout << "AI took : " << (int)_iaElapsedMs << " ms to play" << std::endl;
            _board.placePiece(result.x, result.y, _whiteStone);
            if (checkFiveStonesAligned(result.x, result.y, _whiteStone))
            {
                std::cout << "GAME OVER" << std::endl;
            }
            checkCapturePair(result.x, result.y, _whiteStone);
            if (_whiteStone.getPairCaptured() >= 5)
            {
                std::cout << "GAME OVER" << std::endl;
            }
            _blackTurn = !_blackTurn;
             _turnStartTime = std::chrono::high_resolution_clock::now();
        }
    }
}

void Game::render()
{
    _board.drawBoard();
    _board.drawPieces();

    DrawRectangleRec(_resetButton, LIGHTGRAY);
    int fontSize = 18;
    const char* btnText = "New Game";
    int textWidth = MeasureText(btnText, fontSize);
    int textHeight = fontSize;
    int textX = _resetButton.x + (_resetButton.width - textWidth) / 2;
    int textY = _resetButton.y + (_resetButton.height - textHeight) / 2;
    DrawText(btnText, textX, textY, fontSize, DARKGRAY);
    
    DrawRectangleRec(_menuButton, ORANGE);
    const char* menuText = "Menu";
    int menuTextWidth = MeasureText(menuText, fontSize);
    int menuTextX = _menuButton.x + (_menuButton.width - menuTextWidth) / 2;
    int menuTextY = _menuButton.y + (_menuButton.height - textHeight) / 2;
    DrawText(menuText, menuTextX, menuTextY, fontSize, WHITE);
    
    int evaluatedScore = _board.getEvaluation();
    char evalText[64];
    snprintf(evalText, sizeof(evalText), "Evaluation: %d", evaluatedScore);
    DrawText(evalText, _resetButton.x + _resetButton.width + 20, _resetButton.y + (_resetButton.height - 16) / 2, 16, DARKGRAY);
    
    auto now = std::chrono::high_resolution_clock::now();
    float timerMs = std::chrono::duration<float, std::milli>(now - _turnStartTime).count();
    char timerText[64];
    snprintf(timerText, sizeof(timerText), "Timer: %.0f ms", timerMs);
    DrawText(timerText, 10, BOARD_HEIGHT + 10, 20, MAROON);
}
