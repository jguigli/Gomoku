#include "../../includes/game.hpp"

Game::Game() : _blackTurn(true), _blackStone(CELL_BLACK), _whiteStone(CELL_WHITE), 
               _aiMode(false), _aiTurn(false), _gameOver(false), _gameStatus("Game in progress")
{
    _resetButton = { BOARD_WIDTH / 2 - 80, BOARD_HEIGHT + 5, 160, 30 };
    _aiToggleButton = { 50, BOARD_HEIGHT + 5, 120, 30 };
    _difficultyButton = { 200, BOARD_HEIGHT + 5, 100, 30 };
    _ai = new AI(CELL_WHITE, 10); // AI plays as white, depth 4
}

Game::~Game() 
{
    delete _ai;
}

void Game::reset()
{
    _board.reset();
    _blackTurn = true;
    _aiTurn = _aiMode && !_blackTurn; // AI turn if AI mode and it's white's turn
    _gameOver = false;
    _gameStatus = "Game in progress";
    _blackStone.resetPairCaptured();
    _whiteStone.resetPairCaptured();
}

bool Game::checkFiveStonesAligned(int x, int y, Player stone)
{
    for (auto& direction : directions)
    {
        int count = 1;

        // FORWARD
        for (int i = 1; i < 5; ++i)
        {
            int nx = x + direction[0] * i;
            int ny = y + direction[1] * i;
            if (_board.isInsideGrid(nx, ny) && _board.getCell(nx, ny) == stone.getStoneColor())
                count++;
            else
                break;
        }

        // BACKWARD
        for (int i = 1; i < 5; ++i)
        {
            int nx = x - direction[0] * i;
            int ny = y - direction[1] * i;
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
    Cell opponentStone = stone.getStoneColor() == CELL_BLACK ? CELL_WHITE : CELL_BLACK;

    for (auto& direction : directions)
    {
        int count = 0;

        // FORWARD
        for (int i = 1; i < 4; ++i)
        {
            int nx = x + direction[0] * i;
            int ny = y + direction[1] * i;
            if (_board.isInsideGrid(nx, ny) && _board.getCell(nx, ny) == opponentStone)
                count++;
            else if (_board.isInsideGrid(nx, ny) && _board.getCell(nx, ny) == stone.getStoneColor() && count == 2)
            {
                _board.emptyCell(x + direction[0] * (i - 1), y + direction[1] * (i - 1));
                _board.emptyCell(x + direction[0] * (i - 2), y + direction[1] * (i - 2));
                stone.addCapturedPair();
                return true;
            }
            else
                break;
        }

        // BACKWARD
        for (int i = 1; i < 5; ++i)
        {
            int nx = x - direction[0] * i;
            int ny = y - direction[1] * i;
            if (_board.isInsideGrid(nx, ny) && _board.getCell(nx, ny) == opponentStone)
                count++;
            else if (_board.isInsideGrid(nx, ny) && _board.getCell(nx, ny) == stone.getStoneColor() && count == 2)
            {
                _board.emptyCell(x - direction[0] * (i - 1), y - direction[1] * (i - 1));
                _board.emptyCell(x - direction[0] * (i - 2), y - direction[1] * (i - 2));
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

bool Game::checkDoubleFreeThree(int x, int y, Player stone) {
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

void Game::handlePlayerMove(int x, int y)
{
    Player currentPlayer = _blackTurn ? _blackStone : _whiteStone;
    
    if (!checkDoubleFreeThree(x, y, currentPlayer) || 
        (checkDoubleFreeThree(x, y, currentPlayer) && checkCapturePair(x, y, currentPlayer)))
    {
        if (_board.placePiece(x, y, currentPlayer))
        {
            checkGameEnd(x, y, currentPlayer);
            
            if (!_gameOver) {
                _blackTurn = !_blackTurn;
                _aiTurn = _aiMode && !_blackTurn; // Set AI turn if AI mode and it's white's turn
            }
        }
    }
}

void Game::handleAIMove()
{
    if (_gameOver) return;
    
    Player aiPlayer = _whiteStone;
    Player opponent = _blackStone;
    
    Move aiMove = _ai->getBestMove(_board, aiPlayer, opponent);
    
    if (aiMove.x != -1 && aiMove.y != -1) {
        if (_board.placePiece(aiMove.x, aiMove.y, aiPlayer)) {
            checkGameEnd(aiMove.x, aiMove.y, aiPlayer);
            
            if (!_gameOver) {
                _blackTurn = !_blackTurn;
                _aiTurn = false; // AI has made its move
            }
        }
    }
}

void Game::checkGameEnd(int x, int y, Player currentPlayer)
{
    if (checkFiveStonesAligned(x, y, currentPlayer))
    {
        _gameOver = true;
        _gameStatus = (currentPlayer.getStoneColor() == CELL_BLACK) ? "Black Wins!" : "White Wins!";
        std::cout << _gameStatus << std::endl;
        return;
    }

    checkCapturePair(x, y, currentPlayer);
    if (currentPlayer.getPairCaptured() >= 5)
    {
        _gameOver = true;
        _gameStatus = (currentPlayer.getStoneColor() == CELL_BLACK) ? "Black Wins by Capture!" : "White Wins by Capture!";
        std::cout << _gameStatus << std::endl;
        return;
    }
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
        else if (CheckCollisionPointRec(mouse, _aiToggleButton))
        {
            toggleAIMode();
            reset(); // Reset game when toggling AI mode
        }
        else if (CheckCollisionPointRec(mouse, _difficultyButton))
        {
            // Cycle through difficulty levels (depth 2-6)
            int currentDepth = _ai->getMaxDepth();
            int newDepth = (currentDepth % 5) + 2; // Cycle between 2-6
            setAIDifficulty(newDepth);
        }
        else if (mouse.y < BOARD_HEIGHT && !_gameOver && !_aiTurn)
        {
            int x = round((mouse.x - BOARD_MARGIN) / static_cast<float>(CELL_SIZE));
            int y = round((mouse.y - BOARD_MARGIN) / static_cast<float>(CELL_SIZE));
            
            handlePlayerMove(x, y);
        }
    }
    
    // Handle AI move if it's AI's turn
    if (_aiTurn && !_gameOver) {
        handleAIMove();
    }
}

void Game::render()
{
    _board.drawBoard();
    _board.drawPieces();

    // Draw UI buttons
    DrawRectangleRec(_resetButton, LIGHTGRAY);
    DrawText("New Game", _resetButton.x + 20, _resetButton.y + 5, 18, DARKGRAY);
    
    DrawRectangleRec(_aiToggleButton, _aiMode ? GREEN : LIGHTGRAY);
    DrawText(_aiMode ? "AI: ON" : "AI: OFF", _aiToggleButton.x + 10, _aiToggleButton.y + 5, 18, DARKGRAY);
    
    DrawRectangleRec(_difficultyButton, LIGHTGRAY);
    DrawText(TextFormat("Depth: %d", _ai->getMaxDepth()), _difficultyButton.x + 10, _difficultyButton.y + 5, 18, DARKGRAY);
    
    // Draw game status
    DrawText(_gameStatus.c_str(), 350, BOARD_HEIGHT + 10, 20, DARKBLUE);
    
    // Draw turn indicator
    if (!_gameOver) {
        const char* turnText = _blackTurn ? "Black's Turn" : "White's Turn";
        DrawText(turnText, 50, 10, 20, _blackTurn ? BLACK : GRAY);
        
        if (_aiMode && !_blackTurn) {
            DrawText("AI is thinking...", 200, 10, 20, RED);
        }
    }
    
    // Draw captured pairs
    DrawText(TextFormat("Black Captured: %d", _blackStone.getPairCaptured()), 500, 10, 16, BLACK);
    DrawText(TextFormat("White Captured: %d", _whiteStone.getPairCaptured()), 500, 30, 16, GRAY);
    
    // Draw AI move time if AI just moved
    if (_aiMode && _ai->getLastMoveTime() > 0) {
        DrawText(TextFormat("AI Move Time: %.1f ms", _ai->getLastMoveTime()), 500, 50, 16, BLUE);
    }
}
