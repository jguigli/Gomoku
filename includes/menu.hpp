#pragma once
#include "raylib.h"
#include "config.hpp"

enum GameMode
{
    PLAYER_VS_PLAYER,
    PLAYER_VS_AI
};

enum Difficulty
{
    BEGINNER = 1,
    NORMAL = 4,
    HARD = 10
};

enum MenuState
{
    MENU_MAIN,
    MENU_DIFFICULTY,
    GAME_PLAYING
};

class Menu
{
    public:
        Menu();
        ~Menu();
        
        void update();
        void render();
        void reset();
        
        MenuState getMenuState() const { return _menuState; }
        GameMode getGameMode() const { return _gameMode; }
        Difficulty getDifficulty() const { return _difficulty; }
        
        bool shouldStartGame() const { return _startGame; }
        void gameStarted() { _startGame = false; }

    private:
        MenuState _menuState;
        GameMode _gameMode;
        Difficulty _difficulty;
        bool _startGame;
        
        Rectangle _pvpButton;
        Rectangle _pvaButton;
        Rectangle _beginnerButton;
        Rectangle _normalButton;
        Rectangle _hardButton;
        Rectangle _backButton;
        
        void initButtons();
        void drawButton(Rectangle button, const char* text, Color bgColor, Color textColor);
        bool isButtonClicked(Rectangle button);
}; 