// #include "raylib.h"
#include "../includes/board.hpp"
#include "../includes/game.hpp"
#include "../includes/menu.hpp"
#include "../includes/heuristic.hpp"
#include "../includes/config.hpp"
#include <cmath>


int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Gomoku");
    SetTargetFPS(60);

    Menu menu;
    Game* game = nullptr;

    while (!WindowShouldClose())
    {
        // Update menu or game based on current state
        if (menu.getMenuState() != GAME_PLAYING) {
            menu.update();
            
            // Check if we should start a new game
            if (menu.shouldStartGame()) {
                if (game != nullptr) {
                    delete game;
                }
                game = new Game(menu.getGameMode(), menu.getDifficulty());
                menu.gameStarted();
            }
            
            // Render menu
            menu.render();
        } else if (game != nullptr) {
            // Update and render game
            game->update();
            
            // Check if we should return to menu
            if (game->shouldReturnToMenu()) {
                menu.reset();
                game->resetMenuFlag();
            }

            BeginDrawing();
            ClearBackground(BEIGE);
            game->render();
            
            // Display current game mode info
            const char* modeText = (menu.getGameMode() == PLAYER_VS_AI) ? "Player vs AI" : "Player vs Player";
            DrawText(modeText, 10, 10, 20, DARKGRAY);
            
            if (menu.getGameMode() == PLAYER_VS_AI) {
                const char* diffText;
                switch (menu.getDifficulty()) {
                    case BEGINNER: diffText = "Difficulty: Beginner"; break;
                    case NORMAL: diffText = "Difficulty: Normal"; break;
                    case HARD: diffText = "Difficulty: Hard"; break;
                }
                DrawText(diffText, 10, 35, 20, DARKGRAY);
            }
            
            EndDrawing();
        }
    }

    // Cleanup
    if (game != nullptr) {
        delete game;
    }

    CloseWindow();
    return 0;
}

