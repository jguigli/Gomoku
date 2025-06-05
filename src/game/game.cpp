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
            if (_board.placePiece(x, y, currentPlayer))
            {
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
