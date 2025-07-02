#include "../../includes/menu.hpp"
#include <iostream>

Menu::Menu() : _menuState(MENU_MAIN), _gameMode(PLAYER_VS_PLAYER), _difficulty(NORMAL), _startGame(false) {
    initButtons();
}

Menu::~Menu() {
}

void Menu::initButtons()
{
    float buttonWidth = 300;
    float buttonHeight = 60;
    float centerX = SCREEN_WIDTH / 2 - buttonWidth / 2;
    float startY = SCREEN_HEIGHT / 2 - 50;
    
    _pvpButton = { centerX, startY, buttonWidth, buttonHeight };
    _pvaButton = { centerX, startY + 80, buttonWidth, buttonHeight };
    
    float diffButtonWidth = 200;
    float diffButtonHeight = 50;
    float diffCenterX = SCREEN_WIDTH / 2 - diffButtonWidth / 2;
    float diffStartY = SCREEN_HEIGHT / 2 - 75;
    
    _beginnerButton = { diffCenterX, diffStartY, diffButtonWidth, diffButtonHeight };
    _normalButton = { diffCenterX, diffStartY + 70, diffButtonWidth, diffButtonHeight };
    _hardButton = { diffCenterX, diffStartY + 140, diffButtonWidth, diffButtonHeight };
    
    _backButton = { 50, SCREEN_HEIGHT - 80, 100, 40 };
}

void Menu::update()
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        switch (_menuState)
        {
            case MENU_MAIN:
                if (isButtonClicked(_pvpButton))
                {
                    _gameMode = PLAYER_VS_PLAYER;
                    _startGame = true;
                    _menuState = GAME_PLAYING;
                }
                else if (isButtonClicked(_pvaButton))
                {
                    _gameMode = PLAYER_VS_AI;
                    _menuState = MENU_DIFFICULTY;
                }
                break;
                
            case MENU_DIFFICULTY:
                if (isButtonClicked(_beginnerButton))
                {
                    _difficulty = BEGINNER;
                    _startGame = true;
                    _menuState = GAME_PLAYING;
                }
                else if (isButtonClicked(_normalButton))
                {
                    _difficulty = NORMAL;
                    _startGame = true;
                    _menuState = GAME_PLAYING;
                }
                else if (isButtonClicked(_hardButton))
                {
                    _difficulty = HARD;
                    _startGame = true;
                    _menuState = GAME_PLAYING;
                } else if (isButtonClicked(_backButton))
                {
                    _menuState = MENU_MAIN;
                }
                break;
                
            case GAME_PLAYING:
                break;
        }
    }
    
    if (IsKeyPressed(KEY_ESCAPE))
    {
        if (_menuState == MENU_DIFFICULTY)
            _menuState = MENU_MAIN;
        else if (_menuState == GAME_PLAYING)
            _menuState = MENU_MAIN;
    }
}

void Menu::render()
{
    BeginDrawing();
    ClearBackground(Color{45, 45, 55, 255});
    
    switch (_menuState)
    {
        case MENU_MAIN:
            {
                const char* title = "GOMOKU";
                int titleFontSize = 72;
                int titleWidth = MeasureText(title, titleFontSize);
                DrawText(title, SCREEN_WIDTH / 2 - titleWidth / 2, 150, titleFontSize, GOLD);
            }
            
            {
                const char* subtitle = "Select Game Mode";
                int subtitleFontSize = 32;
                int subtitleWidth = MeasureText(subtitle, subtitleFontSize);
                DrawText(subtitle, SCREEN_WIDTH / 2 - subtitleWidth / 2, 250, subtitleFontSize, LIGHTGRAY);
            }
            
            drawButton(_pvpButton, "Player vs Player", DARKBLUE, WHITE);
            drawButton(_pvaButton, "Player vs AI", DARKGREEN, WHITE);
            break;
            
        case MENU_DIFFICULTY:
            {
                const char* title = "Select Difficulty";
                int titleFontSize = 48;
                int titleWidth = MeasureText(title, titleFontSize);
                DrawText(title, SCREEN_WIDTH / 2 - titleWidth / 2, 200, titleFontSize, GOLD);
            }
            
            drawButton(_beginnerButton, "Beginner", DARKGREEN, WHITE);
            drawButton(_normalButton, "Normal", ORANGE, WHITE);
            drawButton(_hardButton, "Hard", MAROON, WHITE);
            
            drawButton(_backButton, "Back", GRAY, WHITE);
            break;
            
        case GAME_PLAYING:
            // This case should not render the menu
            break;
    }
    
    EndDrawing();
}

void Menu::drawButton(Rectangle button, const char* text, Color bgColor, Color textColor)
{
    Vector2 mousePos = GetMousePosition();
    bool isHovered = CheckCollisionPointRec(mousePos, button);
    
    Color buttonColor = isHovered ? 
        Color{
            static_cast<unsigned char>(bgColor.r + 30 > 255 ? 255 : bgColor.r + 30), 
            static_cast<unsigned char>(bgColor.g + 30 > 255 ? 255 : bgColor.g + 30), 
            static_cast<unsigned char>(bgColor.b + 30 > 255 ? 255 : bgColor.b + 30), 
            255
        } : bgColor;
    
    DrawRectangleRec(button, buttonColor);
    DrawRectangleLinesEx(button, 2, isHovered ? WHITE : LIGHTGRAY);
    
    int fontSize = 24;
    int textWidth = MeasureText(text, fontSize);
    int textHeight = fontSize;
    int textX = button.x + (button.width - textWidth) / 2;
    int textY = button.y + (button.height - textHeight) / 2;
    
    DrawText(text, textX, textY, fontSize, textColor);
}

bool Menu::isButtonClicked(Rectangle button)
{
    Vector2 mousePos = GetMousePosition();
    return CheckCollisionPointRec(mousePos, button);
}

void Menu::reset()
{
    _menuState = MENU_MAIN;
    _gameMode = PLAYER_VS_PLAYER;
    _difficulty = NORMAL;
    _startGame = false;
} 