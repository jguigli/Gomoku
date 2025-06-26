#include "../../includes/game.hpp"


Game::Game() : _blackTurn(true), _blackStone(CELL_BLACK), _whiteStone(CELL_WHITE), _agent(nullptr), _agentActive(false), _gameMode(PLAYER_VS_PLAYER), _difficulty(NORMAL), _backToMenu(false), _currentMoveIndex(-1), _reviewMode(false)
{
    _resetButton = { BOARD_WIDTH / 2 - 120, BOARD_HEIGHT - 25, 100, 40 };
    _menuButton = { BOARD_WIDTH / 2, BOARD_HEIGHT - 25, 100, 40 };
    _backwardButton = { BOARD_WIDTH / 2 - 220, BOARD_HEIGHT - 25, 40, 40 };
    _forwardButton = { BOARD_WIDTH / 2 + 150, BOARD_HEIGHT - 25, 40, 40 };
    _agent = new Agent(static_cast<int>(_difficulty), CELL_WHITE, _blackStone, _whiteStone);
    std::cout<< "CELL_WHITE : " << CELL_WHITE << std::endl;
    _turnStartTime = std::chrono::high_resolution_clock::now();
}

Game::Game(GameMode gameMode, Difficulty difficulty) : _blackTurn(true), _blackStone(CELL_BLACK), _whiteStone(CELL_WHITE), _agent(nullptr), _agentActive(false), _gameMode(gameMode), _difficulty(difficulty), _backToMenu(false), _currentMoveIndex(-1), _reviewMode(false)
{
    _resetButton = { BOARD_WIDTH / 2 - 120, BOARD_HEIGHT - 25, 100, 40 };
    _menuButton = { BOARD_WIDTH / 2, BOARD_HEIGHT - 25, 100, 40 };
    _backwardButton = { BOARD_WIDTH / 2 - 220, BOARD_HEIGHT - 25, 40, 40 };
    _forwardButton = { BOARD_WIDTH / 2 + 150, BOARD_HEIGHT - 25, 40, 40 };
    _agent = new Agent(static_cast<int>(_difficulty), CELL_WHITE, _blackStone, _whiteStone);
    _agentActive = (gameMode == PLAYER_VS_AI);
    _turnStartTime = std::chrono::high_resolution_clock::now();
}

Game::~Game() {
    delete _agent;
}

void Game::addMoveToHistory(int x, int y, Cell stone, const std::vector<std::pair<int, int>>& capturedStones) {
    Move move;
    move.x = x;
    move.y = y;
    move.stone = stone;
    move.moveNumber = _moveHistory.size() + 1;
    move.capturedStones = capturedStones;
    _moveHistory.push_back(move);
    _currentMoveIndex = _moveHistory.size() - 1;
}

void Game::enterReviewMode() {
    if (!_reviewMode)
    {
        _reviewBoard = _board;
        _reviewMode = true;
    }
}

void Game::exitReviewMode()
{
    if (_reviewMode)
        _reviewMode = false;
}

void Game::navigateToMove(int moveIndex)
{
    if (moveIndex < -1 || moveIndex >= static_cast<int>(_moveHistory.size()))
        return;
    
    if (!_reviewMode)
        enterReviewMode();
    
    _currentMoveIndex = moveIndex;
    
    if (moveIndex == static_cast<int>(_moveHistory.size()) - 1)
        exitReviewMode();
    

    _reviewBoard.reset();
    for (int i = 0; i <= moveIndex; i++) {
        Move& move = _moveHistory[i];
        _reviewBoard.setCell(move.x, move.y, move.stone);
        
        // Remove captured stones for this move
        for (const auto& capturedPos : move.capturedStones) {
            _reviewBoard.emptyCell(capturedPos.first, capturedPos.second);
        }
    }
}

void Game::setGameMode(GameMode gameMode, Difficulty difficulty)
{
    _gameMode = gameMode;
    _difficulty = difficulty;
    _agentActive = (gameMode == PLAYER_VS_AI);
    
    delete _agent;
    _agent = new Agent(static_cast<int>(_difficulty), CELL_WHITE, _blackStone, _whiteStone);
    
    reset();
}

void Game::reset()
{
    _board.reset();
    _blackTurn = true;
    _blackStone.resetPairCaptured();
    _whiteStone.resetPairCaptured();
    _moveHistory.clear();
    _currentMoveIndex = -1;
    _reviewMode = false;
    _gameOver = false;
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

bool Game::checkCapturePair(int x, int y, Player& stone)
{
    const int directions[4][2] = {
        {1, 0},   // horizontal
        {0, 1},   // vertical
        {1, 1},   // diagonal gauche droite
        {1, -1}   // diagonal droite gauche
    };

    Cell opponentStone = stone.getStoneColor() == CELL_BLACK ? CELL_WHITE : CELL_BLACK;
    std::vector<std::pair<int, int>> capturedStones;

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
                
                capturedStones.push_back({x + dir[0] * (i - 1), y + dir[1] * (i - 1)});
                capturedStones.push_back({x + dir[0] * (i - 2), y + dir[1] * (i - 2)});
                
                stone.addCapturedPair();
                std::cout << "Capture" << std::endl;
                
                // Update the last move in history with captured stones
                if (!_moveHistory.empty())
                {
                    _moveHistory.back().capturedStones.insert(
                        _moveHistory.back().capturedStones.end(),
                        capturedStones.begin(),
                        capturedStones.end()
                    );
                }
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
                
                capturedStones.push_back({x - dir[0] * (i - 1), y - dir[1] * (i - 1)});
                capturedStones.push_back({x - dir[0] * (i - 2), y - dir[1] * (i - 2)});
                
                stone.addCapturedPair();
                
                // Update the last move in history with captured stones
                if (!_moveHistory.empty())
                {
                    _moveHistory.back().capturedStones.insert(
                        _moveHistory.back().capturedStones.end(),
                        capturedStones.begin(),
                        capturedStones.end()
                    );
                }
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
        else if (CheckCollisionPointRec(mouse, _backwardButton))
        {
            if (_currentMoveIndex > -1) {
                navigateToMove(_currentMoveIndex - 1);
            }
        }
        else if (CheckCollisionPointRec(mouse, _forwardButton))
        {
            if (_currentMoveIndex < static_cast<int>(_moveHistory.size()) - 1) {
                navigateToMove(_currentMoveIndex + 1);
            }
        }
        else if (mouse.y < BOARD_HEIGHT && !_reviewMode && !_gameOver)
        {
            int x = round((mouse.x - BOARD_MARGIN) / static_cast<float>(CELL_SIZE));
            int y = round((mouse.y - BOARD_MARGIN) / static_cast<float>(CELL_SIZE));
            
            Player& currentPlayer = _blackTurn ? _blackStone : _whiteStone;
            if (handlePlayerMove(x, y, currentPlayer))
            {
                addMoveToHistory(x, y, currentPlayer.getStoneColor());
                
                auto now = std::chrono::high_resolution_clock::now();
                _elapsedMs = std::chrono::duration<float, std::milli>(now - _turnStartTime).count();
                _turnStartTime = now;
                std::cout << "Human player took : " << (int)_elapsedMs << " ms to play" << std::endl;
                if (checkFiveStonesAligned(x, y, currentPlayer))
                {
                    std::cout << "GAME OVER" << std::endl;
                    _gameOver = true;
                }
                
                checkCapturePair(x, y, currentPlayer);
                if (currentPlayer.getPairCaptured() >= 5)
                {
                    std::cout << "GAME OVER" << std::endl;
                    _gameOver = true;
                }
                _blackTurn = !_blackTurn;
                _turnStartTime = std::chrono::high_resolution_clock::now();
            }
        }
    }

    if (_reviewMode && _currentMoveIndex == static_cast<int>(_moveHistory.size()) - 1) {
        exitReviewMode();
    }

    if (_agentActive && !_blackTurn && !_reviewMode && !_gameOver)
    {
        auto boardVec = _board.getGrid();
        auto result = _agent->play(boardVec);
        if (result.movePlayed)
        {
            addMoveToHistory(result.x, result.y, CELL_WHITE);
            
            _iaElapsedMs = result.elapsedMs;
            std::cout << "AI took : " << (int)_iaElapsedMs << " ms to play" << std::endl;
            _board.placePiece(result.x, result.y, _whiteStone);
            if (checkFiveStonesAligned(result.x, result.y, _whiteStone))
            {
                std::cout << "GAME OVER" << std::endl;
                _gameOver = true;
            }
            checkCapturePair(result.x, result.y, _whiteStone);
            if (_whiteStone.getPairCaptured() >= 5)
            {
                std::cout << "GAME OVER" << std::endl;
                _gameOver = true;
            }
            _blackTurn = !_blackTurn;
            _turnStartTime = std::chrono::high_resolution_clock::now();
        }
    }
}

void Game::displayInfo()
{
    // Reset button
    DrawRectangleRec(_resetButton, LIGHTGRAY);
    int fontSize = 18;
    const char* btnText = "New Game";
    int textWidth = MeasureText(btnText, fontSize);
    int textHeight = fontSize;
    int textX = _resetButton.x + (_resetButton.width - textWidth) / 2;
    int textY = _resetButton.y + (_resetButton.height - textHeight) / 2;
    DrawText(btnText, textX, textY, fontSize, DARKGRAY);
    
    // Menu button
    DrawRectangleRec(_menuButton, ORANGE);
    const char* menuText = "Menu";
    int menuTextWidth = MeasureText(menuText, fontSize);
    int menuTextX = _menuButton.x + (_menuButton.width - menuTextWidth) / 2;
    int menuTextY = _menuButton.y + (_menuButton.height - textHeight) / 2;
    DrawText(menuText, menuTextX, menuTextY, fontSize, WHITE);
    
    // Backward button
    Color backwardColor = (_currentMoveIndex > -1) ? DARKBLUE : GRAY;
    DrawRectangleRec(_backwardButton, backwardColor);
    DrawText("<", _backwardButton.x + 15, _backwardButton.y + 10, 20, WHITE);
    
    // Forward button
    Color forwardColor = (_currentMoveIndex < static_cast<int>(_moveHistory.size()) - 1) ? DARKBLUE : GRAY;
    DrawRectangleRec(_forwardButton, forwardColor);
    DrawText(">", _forwardButton.x + 15, _forwardButton.y + 10, 20, WHITE);
    
    // Player scores and information panel
    float panelX = static_cast<float>(BOARD_WIDTH + 10);
    float panelY = 50.0f;
    float panelWidth = static_cast<float>(SCREEN_WIDTH - BOARD_WIDTH - 20);
    
    // Player 1 (Black) score panel
    Rectangle player1Panel = { panelX, panelY, panelWidth, 120.0f };
    Color player1Color = _blackTurn ? Color{50, 50, 50, 200} : Color{30, 30, 30, 150};
    DrawRectangleRec(player1Panel, player1Color);
    DrawRectangleLinesEx(player1Panel, 2, _blackTurn ? WHITE : GRAY);
    
    // Player 1 info
    const char* player1Name = "Player 1";
    DrawText(player1Name, static_cast<int>(panelX + 10), static_cast<int>(panelY + 10), 20, WHITE);
    
    // Black stone indicator
    DrawCircle(static_cast<int>(panelX + 20 + MeasureText(player1Name, 20) + 10), static_cast<int>(panelY + 20), 8, BLACK);
    DrawCircleLines(static_cast<int>(panelX + 20 + MeasureText(player1Name, 20) + 10), static_cast<int>(panelY + 20), 8, WHITE);
    
    // Player 1 captured pairs
    char player1Score[32];
    snprintf(player1Score, sizeof(player1Score), "Captured Pairs: %d/5", _blackStone.getPairCaptured());
    DrawText(player1Score, static_cast<int>(panelX + 10), static_cast<int>(panelY + 40), 16, LIGHTGRAY);
    
    // Current turn indicator for Player 1
    if (_blackTurn && !_reviewMode && !_gameOver) {
        DrawText("YOUR TURN", static_cast<int>(panelX + 10), static_cast<int>(panelY + 65), 18, GOLD);
    }
    
    // Player 2 (White/AI) score panel
    Rectangle player2Panel = { panelX, panelY + 140.0f, panelWidth, 120.0f };
    Color player2Color = !_blackTurn ? Color{50, 50, 50, 200} : Color{30, 30, 30, 150};
    DrawRectangleRec(player2Panel, player2Color);
    DrawRectangleLinesEx(player2Panel, 2, !_blackTurn ? WHITE : GRAY);
    
    // Player 2 info
    const char* player2Name = _agentActive ? "AI Player" : "Player 2";
    DrawText(player2Name, static_cast<int>(panelX + 10), static_cast<int>(panelY + 150), 20, WHITE);
    
    // White stone indicator
    DrawCircle(static_cast<int>(panelX + 20 + MeasureText(player2Name, 20) + 10), static_cast<int>(panelY + 160), 8, WHITE);
    DrawCircleLines(static_cast<int>(panelX + 20 + MeasureText(player2Name, 20) + 10), static_cast<int>(panelY + 160), 8, BLACK);
    
    // Player 2 captured pairs
    char player2Score[32];
    snprintf(player2Score, sizeof(player2Score), "Captured Pairs: %d/5", _whiteStone.getPairCaptured());
    DrawText(player2Score, static_cast<int>(panelX + 10), static_cast<int>(panelY + 180), 16, LIGHTGRAY);
    
    // Current turn indicator for Player 2
    if (!_blackTurn && !_reviewMode && !_gameOver) {
        const char* turnText = _agentActive ? "AI THINKING..." : "YOUR TURN";
        DrawText(turnText, static_cast<int>(panelX + 10), static_cast<int>(panelY + 215), 18, GOLD);
    }
    
    // Review mode indicator
    if (_reviewMode) {
        DrawText("REVIEW MODE", static_cast<int>(panelX + 10), static_cast<int>(panelY + 70), 16, ORANGE);
        char moveText[32];
        snprintf(moveText, sizeof(moveText), "Move: %d/%d", _currentMoveIndex + 1, static_cast<int>(_moveHistory.size()));
        DrawText(moveText, static_cast<int>(panelX + 10), static_cast<int>(panelY + 90), 16, ORANGE);
    }
    
    // Game mode and difficulty info
    int infoY = static_cast<int>(panelY + 280);
    const char* modeText = _agentActive ? "Mode: Player vs AI" : "Mode: Player vs Player";
    DrawText(modeText, static_cast<int>(panelX + 10), infoY, 16, DARKGRAY);
    
    if (_agentActive) {
        const char* diffText;
        switch (_difficulty) {
            case BEGINNER: diffText = "Difficulty: Beginner"; break;
            case NORMAL: diffText = "Difficulty: Normal"; break;
            case HARD: diffText = "Difficulty: Hard"; break;
        }
        DrawText(diffText, static_cast<int>(panelX + 10), infoY + 25, 16, DARKGRAY);
    }
    
    // Performance info
    int evaluatedScore = _board.getEvaluation();
    char evalText[64];
    snprintf(evalText, sizeof(evalText), "Board Evaluation: %d", evaluatedScore);
    DrawText(evalText, static_cast<int>(panelX + 10), infoY + 55, 14, DARKGRAY);
    
    // Timer for current turn
    if (!_reviewMode) {
        auto now = std::chrono::high_resolution_clock::now();
        float timerMs = std::chrono::duration<float, std::milli>(now - _turnStartTime).count();
        if (_gameOver)
            timerMs = 0;
        char timerText[64];
        snprintf(timerText, sizeof(timerText), "Turn Time: %.1f s", timerMs / 1000.0f);
        DrawText(timerText, static_cast<int>(panelX + 10), infoY + 80, 14, MAROON);
    }
    
    // Last move time
    if (_elapsedMs > 0) {
        char lastMoveText[64];
        snprintf(lastMoveText, sizeof(lastMoveText), "Last Human: %.0f ms", _elapsedMs);
        DrawText(lastMoveText, static_cast<int>(panelX + 10), infoY + 105, 14, DARKBLUE);
    }
    
    if (_iaElapsedMs > 0 && _agentActive) {
        char lastAiText[64];
        snprintf(lastAiText, sizeof(lastAiText), "Last AI: %.0f ms", _iaElapsedMs);
        DrawText(lastAiText, static_cast<int>(panelX + 10), infoY + 130, 14, DARKGREEN);
    }

    if (_gameOver)
    {
        char gameOverText[64];
        snprintf(gameOverText, sizeof(gameOverText), "GAME OVER");
        DrawText(gameOverText, static_cast<int>(panelX + 10), infoY + 170, 20, RED);

        if(!_reviewMode)
        {
            if (_blackTurn)
            {
                char playerWinnerText[64];
                snprintf(playerWinnerText, sizeof(playerWinnerText), "LOSER");
                DrawText(playerWinnerText, static_cast<int>(panelX + 10), panelY + 90, 20, RED);
                snprintf(playerWinnerText, sizeof(playerWinnerText), "WINNER");
                DrawText(playerWinnerText, static_cast<int>(panelX + 10), panelY + 230, 20, GREEN);
            }
            else
            {
                char playerWinnerText[64];
                snprintf(playerWinnerText, sizeof(playerWinnerText), "WINNER");
                DrawText(playerWinnerText, static_cast<int>(panelX + 10), panelY + 90, 20, GREEN);
                snprintf(playerWinnerText, sizeof(playerWinnerText), "LOSER");
                DrawText(playerWinnerText, static_cast<int>(panelX + 10), panelY + 230, 20, RED);
            }
        }

    }
}

void Game::drawMoveHighlight()
{
    if (_reviewMode && _currentMoveIndex >= 0 && _currentMoveIndex < static_cast<int>(_moveHistory.size())) {
        Move& currentMove = _moveHistory[_currentMoveIndex];
        
        int drawX = BOARD_MARGIN + currentMove.x * CELL_SIZE;
        int drawY = BOARD_MARGIN + currentMove.y * CELL_SIZE;
        DrawRectangleLines(drawX - CELL_SIZE/2, drawY - CELL_SIZE/2, 
                            CELL_SIZE, CELL_SIZE, YELLOW);
    }
}

void Game::render()
{
    if (_reviewMode)
    {
        _reviewBoard.drawBoard();
        _reviewBoard.drawPieces();
    }
    else
    {
        _board.drawBoard();
        _board.drawPieces();
    }
    
    drawMoveHighlight();
    displayInfo();
}
