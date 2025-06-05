#include "raylib.h"
#include <cstdio>
#include <cmath>
#include "../includes/ai.hpp"

const int screenWidth = 800;
const int screenHeight = 800;

extern std::vector<std::vector<Cell>> board;
extern const int gridSize;
extern const int margin;
extern const int cellSize;

int evaluatedScore = 0;
auto turnStart = std::chrono::high_resolution_clock::now();
float elapsedMs = 0.0f;

int getEvaluation() {
    std::vector<std::vector<int>> intBoard(gridSize, std::vector<int>(gridSize, 0));
    for (int y = 0; y < gridSize; ++y)
        for (int x = 0; x < gridSize; ++x)
            intBoard[y][x] = board[y][x];
    int scoreWhite = evaluate_board(intBoard, CELL_WHITE);
    int scoreBlack = evaluate_board(intBoard, CELL_BLACK);
    return scoreWhite - scoreBlack;
}

int main() {
    InitWindow(screenWidth, screenHeight + 60, "Gomoku");
    SetTargetFPS(60);

    int textWidth = MeasureText("New Game", 16);
    int textHeight = 16;

    bool iaActive = false;

    Rectangle buttonReset = { screenWidth / 2.0f - 60, screenHeight + 10, 120, 40 };

    evaluatedScore = getEvaluation();

    while (!WindowShouldClose()) {

        auto now = std::chrono::high_resolution_clock::now();
        elapsedMs = std::chrono::duration<float, std::milli>(now - turnStart).count();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mouse = GetMousePosition();

            if (CheckCollisionPointRec(mouse, buttonReset)) {
                ResetBoard();
                evaluatedScore = getEvaluation();
                turnStart = std::chrono::high_resolution_clock::now();
            } else if (mouse.y < screenHeight) {
                int gridX = round((mouse.x - margin) / (float)cellSize);
                int gridY = round((mouse.y - margin) / (float)cellSize);
                PlacePiece(gridX, gridY);
                evaluatedScore = getEvaluation();
                printf ("Human player took %.0f ms\n", elapsedMs);
                turnStart = std::chrono::high_resolution_clock::now();
            }
        }
        if (IsKeyPressed(KEY_I)) {
            iaActive = !iaActive;
        }
        if (iaActive && !playerTurn) {
            // std::vector<std::vector<int>> intBoard(gridSize, std::vector<int>(gridSize, 0));
            // for (int y = 0; y < gridSize; ++y)
            //     for (int x = 0; x < gridSize; ++x)
            //         intBoard[y][x] = board[y][x];

            // int bestY = -1, bestX = -1, bestScore = std::numeric_limits<int>::min();
            // int profondeur = 1;
            // Cell JoueurIA = CELL_WHITE;
            // for (auto [y, x] : generate_moves(intBoard)) {
            //     intBoard[y][x] = JoueurIA;
            //     int score = minimax(intBoard, profondeur, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), true, JoueurIA, JoueurIA);
            //     intBoard[y][x] = 0;
            //     if (score > bestScore) {
            //         bestScore = score;
            //         bestY = y;
            //         bestX = x;
            //     }
            // }
            // auto iaNow = std::chrono::high_resolution_clock::now();
            // float iaElapsedMs = std::chrono::duration<float, std::milli>(iaNow - turnStart).count();
            // if (bestY != -1 && bestX != -1) {
            //     board[bestY][bestX] = JoueurIA;
            //     evaluatedScore = getEvaluation();
            //     playerTurn = !playerTurn;
            //     printf ("AI took %.0f ms\n", iaElapsedMs);
            //     turnStart = std::chrono::high_resolution_clock::now();
            // }
            int profondeur = 2;
            Cell JoueurIA = CELL_WHITE;
            AIMoveResult result = play_ai_turn(board, profondeur, JoueurIA);
            if (result.movePlayed) {
                evaluatedScore = getEvaluation();
                playerTurn = !playerTurn;
                printf("AI took %.0f ms\n", result.elapsedMs);
                turnStart = std::chrono::high_resolution_clock::now();
            }
        }

        BeginDrawing();
        ClearBackground(BEIGE);

        DrawBoard();
        DrawPieces();

        char timerText[32];
        snprintf(timerText, sizeof(timerText), "Timer: %.0f ms", elapsedMs);
        DrawText(timerText, 20, screenHeight + 20, 20, DARKGRAY);

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
