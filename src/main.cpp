// #include "raylib.h"
// #include <vector>
// #include <cmath>

// const int screenWidth = 800;
// const int screenHeight = 800;
// const int gridSize = 19;
// const int margin = 40;
// const int cellSize = (screenWidth - 2 * margin) / (gridSize - 1);

// enum Cell { EMPTY, CELL_BLACK, CELL_WHITE };

// std::vector<std::vector<Cell>> board(gridSize, std::vector<Cell>(gridSize, EMPTY));
// bool playerTurn = true; // true = black, false = white

// void ResetBoard() {
//     for (auto &row : board) {
//         std::fill(row.begin(), row.end(), EMPTY);
//     }
//     playerTurn = true;
// }

// bool IsInsideBoard(int x, int y) {
//     return x >= 0 && x < gridSize && y >= 0 && y < gridSize;
// }

// bool PlacePiece(int x, int y) {
//     if (IsInsideBoard(x, y) && board[y][x] == EMPTY) {
//         board[y][x] = playerTurn ? CELL_BLACK : CELL_WHITE;
//         playerTurn = !playerTurn;
//         return true;
//     }
//     return false;
// }

// void DrawBoard() {
//     for (int i = 0; i < gridSize; i++) {
//         int pos = margin + i * cellSize;
//         DrawLine(margin, pos, screenWidth - margin, pos, DARKGRAY);   // horizontal
//         DrawLine(pos, margin, pos, screenHeight - margin, DARKGRAY);  // vertical
//     }
// }

// void DrawPieces() {
//     for (int y = 0; y < gridSize; y++) {
//         for (int x = 0; x < gridSize; x++) {
//             int px = margin + x * cellSize;
//             int py = margin + y * cellSize;
//             if (board[y][x] == CELL_BLACK) {
//                 DrawCircle(px, py, cellSize / 3, BLACK);
//             } else if (board[y][x] == CELL_WHITE) {
//                 DrawCircle(px, py, cellSize / 3, RAYWHITE);
//                 DrawCircleLines(px, py, cellSize / 3, GRAY);
//             }
//         }
//     }
// }

// int main() {
//     InitWindow(screenWidth, screenHeight + 60, "Gomoku");
//     SetTargetFPS(60);

//     int textWidth = MeasureText("New Game", 16);
//     int textHeight = 16;

//     Rectangle buttonReset = { screenWidth / 2.0f - 60, screenHeight + 10, 120, 40 };

//     while (!WindowShouldClose()) {
//         if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
//             Vector2 mouse = GetMousePosition();

//             if (CheckCollisionPointRec(mouse, buttonReset)) {
//                 ResetBoard();
//             } else if (mouse.y < screenHeight) {
//                 int gridX = round((mouse.x - margin) / (float)cellSize);
//                 int gridY = round((mouse.y - margin) / (float)cellSize);
//                 PlacePiece(gridX, gridY);
//             }
//         }

//         BeginDrawing();
//         ClearBackground(BEIGE);

//         DrawBoard();
//         DrawPieces();

//         DrawRectangleRec(buttonReset, LIGHTGRAY);
//         DrawText("New Game", buttonReset.x + (buttonReset.width - textWidth) / 2, 
//         buttonReset.y + (buttonReset.height - textHeight) / 2, 16, DARKGRAY);

//         EndDrawing();
//     }

//     CloseWindow();
//     return 0;
// }

#include "raylib.h"
#include "board.hpp"
#include <cmath>
const int screenWidth = 800;
const int screenHeight = 800;

int main() {
    InitWindow(screenWidth, screenHeight + 60, "Gomoku");
    SetTargetFPS(60);

    int textWidth = MeasureText("New Game", 16);
    int textHeight = 16;

    Rectangle buttonReset = { screenWidth / 2.0f - 60, screenHeight + 10, 120, 40 };

    while (!WindowShouldClose()) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mouse = GetMousePosition();

            if (CheckCollisionPointRec(mouse, buttonReset)) {
                ResetBoard();
            } else if (mouse.y < screenHeight) {
                int gridX = round((mouse.x - margin) / (float)cellSize);
                int gridY = round((mouse.y - margin) / (float)cellSize);
                PlacePiece(gridX, gridY);
            }
        }

        BeginDrawing();
        ClearBackground(BEIGE);

        DrawBoard();
        DrawPieces();

        DrawRectangleRec(buttonReset, LIGHTGRAY);
        DrawText("New Game", buttonReset.x + (buttonReset.width - textWidth) / 2, 
                 buttonReset.y + (buttonReset.height - textHeight) / 2, 16, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
