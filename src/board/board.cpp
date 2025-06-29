#include "../../includes/board.hpp"
#include "../../includes/heuristic.hpp"


Board::Board()
{
    _grid.resize(GRID_SIZE, std::vector<Cell>(GRID_SIZE, CELL_EMPTY));
}

Board::~Board() {}

void Board::reset()
{
    for (auto& row : _grid)
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
            if (_grid[y][x] == CELL_BLACK)
            {
                DrawCircle(px, py, CELL_SIZE / 2.3, BLACK);
            }
            else if (_grid[y][x] == CELL_WHITE)
            {
                DrawCircle(px, py, CELL_SIZE / 2.3, RAYWHITE);
                DrawCircleLines(px, py, CELL_SIZE / 2.3, GRAY);
            }
        }
    }
}

bool Board::placePiece(int x, int y, Player stone)
{
    if (isInsideGrid(x, y) && _grid[y][x] == CELL_EMPTY)
    {
        _grid[y][x] = stone.getStoneColor();
        
        // for (auto& row : _grid)
        // {
        //     for (int y = 0; y < 19; ++y)
        //         std::cout << row[y];
        //     std::cout << std::endl;
        // }

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
    return _grid[y][x];
}

void Board::setCell(int x, int y, Cell cell)
{
    if (isInsideGrid(x, y))
        _grid[y][x] = cell;
}

void Board::emptyCell(int x, int y)
{
    if (isInsideGrid(x, y))
        _grid[y][x] = CELL_EMPTY;
}


int Board::getEvaluation()
{
    std::vector<std::vector<int>> intBoard(GRID_SIZE, std::vector<int>(GRID_SIZE, 0));

    for (int y = 0; y < GRID_SIZE; ++y)
    {
        for (int x = 0; x < GRID_SIZE; ++x)
        {
            if (_grid[y][x] == CELL_BLACK)
                intBoard[y][x] = 1;
            else if (_grid[y][x] == CELL_WHITE)
                intBoard[y][x] = 2;
            else
                intBoard[y][x] = 0;
        }
    }

    int score = evaluate_board(intBoard, 2, 2, 0, 0);

    return score;
}

const std::vector<std::vector<Cell>>& Board::getGrid() const
{
    return _grid;
}