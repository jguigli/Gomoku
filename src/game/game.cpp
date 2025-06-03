#include "../includes/game.hpp"

Game::Game() : blackTurn(true)
{
    resetButton = { 320, 810, 160, 40 };
}

void Game::reset()
{
    board.reset();
    blackTurn = true;
}

bool Game::checkFiveStoneAligned()
{

}

bool Game::checkCapturePair()
{

}

void Game::update()
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse = GetMousePosition();
        if (CheckCollisionPointRec(mouse, resetButton)) {
            reset();
        } else if (mouse.y < 800) {
            int x = mouse.x / board.getCellSize();
            int y = mouse.y / board.getCellSize();
            Cell currentPlayer = blackTurn ? Cell::BLACK : Cell::WHITE;
            if (board.placePiece(x, y, currentPlayer)) {
                blackTurn = !blackTurn;
            }
        }
    }
}

void Game::render() {
    board.draw();
    board.drawPieces();

    DrawRectangleRec(resetButton, LIGHTGRAY);
    DrawText("New Game", resetButton.x + 10, resetButton.y + 10, 18, DARKGRAY);
}
