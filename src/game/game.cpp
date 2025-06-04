#include "../../includes/game.hpp"


Game::Game() : blackTurn(true)
{
    resetButton = { BOARD_WIDTH / 2 - 60, BOARD_HEIGHT - 25, 120, 40 };
}

Game::~Game() {}

void Game::reset()
{
    board.reset();
    blackTurn = true;
}

bool Game::checkFiveStoneAligned()
{
    return true; ///////////////////////
}

bool Game::checkCapturePair()
{
    return true; ///////////////////////
}

void Game::update()
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mouse = GetMousePosition();
        if (CheckCollisionPointRec(mouse, resetButton))
        {
            reset();
        }
        else if (mouse.y < BOARD_HEIGHT)
        {
            int x = round((mouse.x - BOARD_MARGIN) / (float)CELL_SIZE);
            int y = round((mouse.y - BOARD_MARGIN) / (float)CELL_SIZE);

            Cell currentPlayer = blackTurn ? CELL_BLACK : CELL_WHITE;
            if (board.placePiece(x, y, currentPlayer))
                blackTurn = !blackTurn;
        }
    }
}

void Game::render()
{
    board.drawBoard();
    board.drawPieces();

    DrawRectangleRec(resetButton, LIGHTGRAY);
    DrawText("New Game", resetButton.x + 10, resetButton.y + 10, 18, DARKGRAY);
}
