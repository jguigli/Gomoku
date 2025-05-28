#include "raylib.h"
#include "../includes/board.hpp"
#include <vector>
#include <algorithm>

const int gridSize = 19;
const int margin = 40;
const int cellSize = (800 - 2 * margin) / (gridSize - 1);

std::vector<std::vector<Cell>> board(gridSize, std::vector<Cell>(gridSize, EMPTY));
bool playerTurn = true;

void ResetBoard() {
    for (auto &row : board) {
        std::fill(row.begin(), row.end(), EMPTY);
    }
    playerTurn = true;
}

bool IsInsideBoard(int x, int y) {
    return x >= 0 && x < gridSize && y >= 0 && y < gridSize;
}

bool PlacePiece(int x, int y) {
    if (IsInsideBoard(x, y) && board[y][x] == EMPTY) {
        board[y][x] = playerTurn ? CELL_BLACK : CELL_WHITE;
        playerTurn = !playerTurn;
        return true;
    }
    return false;
}

void DrawBoard() {
    for (int i = 0; i < gridSize; i++) {
        int pos = margin + i * cellSize;
        DrawLine(margin, pos, 800 - margin, pos, DARKGRAY);
        DrawLine(pos, margin, pos, 800 - margin, DARKGRAY);
    }
}

void DrawPieces() {
    for (int y = 0; y < gridSize; y++) {
        for (int x = 0; x < gridSize; x++) {
            int px = margin + x * cellSize;
            int py = margin + y * cellSize;
            if (board[y][x] == CELL_BLACK) {
                DrawCircle(px, py, cellSize / 3, BLACK);
            } else if (board[y][x] == CELL_WHITE) {
                DrawCircle(px, py, cellSize / 3, RAYWHITE);
                DrawCircleLines(px, py, cellSize / 3, GRAY);
            }
        }
    }
}