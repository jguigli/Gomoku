#include "../../includes/game.hpp"


Game::Game() : _blackTurn(true), _blackStone(CELL_BLACK), _whiteStone(CELL_WHITE)
{
    _resetButton = { BOARD_WIDTH / 2 - 60, BOARD_HEIGHT - 25, 120, 40 };
}

Game::~Game() {}

void Game::reset()
{
    _board.reset();
    _blackTurn = true;
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
			continue;

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

void Game::update()
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mouse = GetMousePosition();
        if (CheckCollisionPointRec(mouse, _resetButton))
        {
            reset();
        }
        else if (mouse.y < BOARD_HEIGHT)
        {
            int x = round((mouse.x - BOARD_MARGIN) / static_cast<float>(CELL_SIZE));
            int y = round((mouse.y - BOARD_MARGIN) / static_cast<float>(CELL_SIZE));

            Player currentPlayer = _blackTurn ? _blackStone : _whiteStone;
            if (!checkDoubleFreeThree(x, y, currentPlayer)) // mauvaise position, car un joueur peut double freethree si il capture une pair
            {
                if (_board.placePiece(x, y, currentPlayer))
                {
                    if (checkFiveStonesAligned(x, y, currentPlayer))
                    {
                        std::cout << "GAME OVER" << std::endl;
                        // rajouter le fait qu'une pair peut etre capturé avant la fin
                    }
    
                    checkCapturePair(x, y, currentPlayer);
                    if (currentPlayer.getPairCaptured() >= 5)
                    {
                        std::cout << "GAME OVER" << std::endl;
                    }
                    _blackTurn = !_blackTurn;
                }
            }
        }
    }
}

void Game::render()
{
    _board.drawBoard();
    _board.drawPieces();

    DrawRectangleRec(_resetButton, LIGHTGRAY);
    DrawText("New Game", _resetButton.x + 10, _resetButton.y + 10, 18, DARKGRAY);
}
