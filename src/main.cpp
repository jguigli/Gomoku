#include "raylib.h"
#include "board.hpp"
#include <cstdio>
#include <cmath>
#include "../includes/heuristic.hpp"

const int screenWidth = 800;
const int screenHeight = 800;

extern std::vector<std::vector<Cell>> board;
extern const int gridSize;
extern const int margin;
extern const int cellSize;

int evaluatedScore = 0;

int getEvaluation() {
    std::vector<std::vector<int>> intBoard(gridSize, std::vector<int>(gridSize, 0));
    for (int y = 0; y < gridSize; ++y)
        for (int x = 0; x < gridSize; ++x)
            intBoard[y][x] = board[y][x];
    int scoreWhite = evaluate_board(intBoard, CELL_WHITE);
    int scoreBlack = evaluate_board(intBoard, CELL_BLACK);
    return scoreBlack - scoreWhite;
}

int main() {
    InitWindow(screenWidth, screenHeight + 60, "Gomoku");
    SetTargetFPS(60);

    int textWidth = MeasureText("New Game", 16);
    int textHeight = 16;

    Rectangle buttonReset = { screenWidth / 2.0f - 60, screenHeight + 10, 120, 40 };

    evaluatedScore = getEvaluation();

    while (!WindowShouldClose()) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mouse = GetMousePosition();

            if (CheckCollisionPointRec(mouse, buttonReset)) {
                ResetBoard();
                evaluatedScore = getEvaluation();
            } else if (mouse.y < screenHeight) {
                int gridX = round((mouse.x - margin) / (float)cellSize);
                int gridY = round((mouse.y - margin) / (float)cellSize);
                PlacePiece(gridX, gridY);
                evaluatedScore = getEvaluation();
            }
        }

        BeginDrawing();
        ClearBackground(BEIGE);

        DrawBoard();
        DrawPieces();

        DrawRectangleRec(buttonReset, LIGHTGRAY);
        DrawText("New Game", buttonReset.x + (buttonReset.width - textWidth) / 2, 
                 buttonReset.y + (buttonReset.height - textHeight) / 2, 16, DARKGRAY);
        
        char evalText[64];
        snprintf(evalText, sizeof(evalText), "Evaluation: %d", evaluatedScore);
        DrawText(evalText, buttonReset.x + buttonReset.width + 20, buttonReset.y + (buttonReset.height - textHeight) / 2, 16, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
