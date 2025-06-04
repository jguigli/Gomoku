// #include "raylib.h"
#include "../includes/board.hpp"
#include "../includes/game.hpp"
#include "../includes/heuristic.hpp"
#include "../includes/config.hpp"
#include <cmath>


int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Gomoku");
    SetTargetFPS(60);

    Game game;

    while (!WindowShouldClose())
    {
        game.update();

        BeginDrawing();
        ClearBackground(BEIGE);
        game.render();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

