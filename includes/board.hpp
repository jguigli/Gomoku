#ifndef BOARD_HPP
#define BOARD_HPP

#include "raylib.h"
#include <vector>

extern const int gridSize;
extern const int margin;
extern const int cellSize;

enum Cell { EMPTY, CELL_BLACK, CELL_WHITE };

extern std::vector<std::vector<Cell>> board;
extern bool playerTurn;

void ResetBoard();
bool IsInsideBoard(int x, int y);
bool PlacePiece(int x, int y);
void DrawBoard();
void DrawPieces();
void manageBoard();

#endif