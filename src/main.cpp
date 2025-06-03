#include "raylib.h"
#include "../includes/board.hpp"
#include "../includes/game.hpp"
#include "../includes/heuristic.hpp"
#include <cmath>

extern std::vector<std::vector<Cell>> board;
extern const int gridSize;
extern const int margin;
extern const int cellSize;

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

