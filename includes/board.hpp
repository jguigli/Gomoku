#pragma once
#include "raylib.h"
#include "config.hpp"
#include <vector>

enum Cell { CELL_EMPTY, CELL_BLACK, CELL_WHITE };

class Board 
{	
	public:
		Board();
		~Board();
		void drawBoard() const;
		void drawPieces() const;
		bool placePiece(int x, int y, Cell player);
		void reset();
		bool isInsideGrid(int x, int y) const;
		Cell getCell(int x, int y) const;
		int getEvaluation();

	private:
		std::vector<std::vector<Cell>> grid;
		// bool playerTurn;
};
