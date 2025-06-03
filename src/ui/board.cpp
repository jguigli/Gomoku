#include "../includes/board.hpp"

Board::Board() {
    grid.resize(gridSize, std::vector<Cell>(gridSize, Cell::EMPTY));
}

void Board::reset()
{
    for (auto& row : grid) {
        std::fill(row.begin(), row.end(), Cell::EMPTY);
    }
}

void Board::drawBoard() const
{
    for (int i = 0; i < gridSize; ++i)
	{
        DrawLine(i * cellSize, 0, i * cellSize, screenSize, DARKGRAY);
        DrawLine(0, i * cellSize, screenSize, i * cellSize, DARKGRAY);
    }
}

void Board::drawPieces() const
{
    for (int y = 0; y < gridSize; ++y)
	{
        for (int x = 0; x < gridSize; ++x)
		{
            Vector2 center = {
                static_cast<float>(x * cellSize + cellSize / 2),
                static_cast<float>(y * cellSize + cellSize / 2)
            };
            if (grid[y][x] == Cell::CELL_BLACK)
			{
                DrawCircleV(center, cellSize / 2.5f, BLACK);
            }
			else if (grid[y][x] == Cell::CELL_WHITE)
			{
                DrawCircleV(center, cellSize / 2.5f, RAYWHITE);
                DrawCircleLines(center.x, center.y, cellSize / 2.5f, GRAY);
            }
        }
    }
}

bool Board::placePiece(int x, int y, Cell player)
{
    if (isInside(x, y) && grid[y][x] == Cell::EMPTY)
	{
        grid[y][x] = player;
        return true;
    }
    return false;
}

bool Board::isInside(int x, int y) const
{
    return x >= 0 && x < gridSize && y >= 0 && y < gridSize;
}

Cell Board::getCell(int x, int y) const
{
    return grid[y][x];
}

int Board::getEvaluation() {
    std::vector<std::vector<int>> intBoard(gridSize, std::vector<int>(gridSize, 0));
    for (int y = 0; y < gridSize; ++y)
        for (int x = 0; x < gridSize; ++x) {
            if (board[y][x] == Cell::CELL_BLACK)
                intBoard[y][x] = 1;
            else if (board[y][x] == Cell::CELL_WHITE)
                intBoard[y][x] = 2;
            else
                intBoard[y][x] = 0;
        }
    int scoreWhite = evaluate_board(intBoard, 2);
    int scoreBlack = evaluate_board(intBoard, 1);
    return scoreWhite - scoreBlack;
}