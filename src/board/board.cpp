#include "../../includes/board.hpp"
#include "../../includes/heuristic.hpp"


Board::Board()
{
    grid.resize(GRID_SIZE, std::vector<Cell>(GRID_SIZE, CELL_EMPTY));
}

Board::~Board() {}

void Board::reset()
{
    for (auto& row : grid)
    {
        std::fill(row.begin(), row.end(), CELL_EMPTY);
    }
}

void Board::drawBoard() const
{
    for (int i = 0; i < GRID_SIZE; i++)
    {
        int pos = BOARD_MARGIN + i * CELL_SIZE;
        DrawLine(BOARD_MARGIN, pos, BOARD_WIDTH - BOARD_MARGIN, pos, DARKGRAY);
        DrawLine(pos, BOARD_MARGIN, pos, BOARD_HEIGHT - BOARD_MARGIN, DARKGRAY);
    }
}

void Board::drawPieces() const 
{
    for (int y = 0; y < GRID_SIZE; y++)
    {
        for (int x = 0; x < GRID_SIZE; x++)
        {
            int px = BOARD_MARGIN + x * CELL_SIZE;
            int py = BOARD_MARGIN + y * CELL_SIZE;
            if (grid[y][x] == CELL_BLACK)
            {
                DrawCircle(px, py, CELL_SIZE / 3, BLACK);
            }
            else if (grid[y][x] == CELL_WHITE)
            {
                DrawCircle(px, py, CELL_SIZE / 3, RAYWHITE);
                DrawCircleLines(px, py, CELL_SIZE / 3, GRAY);
            }
        }
    }
}

bool Board::placePiece(int x, int y, Cell player)
{
    if (isInsideGrid(x, y) && grid[y][x] == CELL_EMPTY)
    {
        grid[y][x] = player;
        // togglePlayer();
        return true;
    }
    return false;
}

bool Board::isInsideGrid(int x, int y) const
{
    return x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE;
}

Cell Board::getCell(int x, int y) const
{
    return grid[y][x];
}


int Board::getEvaluation()
{
    std::vector<std::vector<int>> intBoard(GRID_SIZE, std::vector<int>(GRID_SIZE, 0));

    for (int y = 0; y < GRID_SIZE; ++y)
    {
        for (int x = 0; x < GRID_SIZE; ++x)
        {
            if (grid[y][x] == CELL_BLACK)
                intBoard[y][x] = 1;
            else if (grid[y][x] == CELL_WHITE)
                intBoard[y][x] = 2;
            else
                intBoard[y][x] = 0;
        }
    }

    int scoreWhite = evaluate_board(intBoard, 2);
    int scoreBlack = evaluate_board(intBoard, 1);

    return scoreWhite - scoreBlack;
}