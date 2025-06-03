#pragma once
#include "raylib.h"
#include <vector>

enum class Cell { EMPTY, CELL_BLACK, CELL_WHITE };

class Board 
{
	private:
		static constexpr int gridSize = 19;
		static constexpr int screenSize = 800;
		static constexpr int cellSize = screenSize / gridSize;
		std::vector<std::vector<Cell>> grid;

	public:
		Board();
		void drawBoard() const;
		void drawPieces() const;
		bool placePiece(int x, int y, Cell player);
		void reset();
		bool isInside(int x, int y) const;
		Cell getCell(int x, int y) const;
		int getGridSize() const { return gridSize; }
		int getCellSize() const { return cellSize; }
};
