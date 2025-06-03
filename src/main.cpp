#include "raylib.h"
// #include "./includes/board.hpp"
#include "./includes/game.hpp"
#include <cmath>

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
    return scoreWhite - scoreBlack;
}

int main()
{
    InitWindow(800, 850, "Gomoku en C++ avec Raylib");
    SetTargetFPS(60);

    Game game;

    while (!WindowShouldClose()) {
        game.update();

        BeginDrawing();
        ClearBackground(BEIGE);
        game.render();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

