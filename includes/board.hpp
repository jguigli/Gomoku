#pragma once
#include "raylib.h"
#include "config.hpp"
#include "player.hpp"
#include <iostream>
#include <vector>

class Board 
{	
	public:
		Board();
		~Board();
		void drawBoard() const;
		void drawPieces() const;
		bool placePiece(int x, int y, Player player);
		void reset();
		bool isInsideGrid(int x, int y) const;
		Cell getCell(int x, int y) const;
		void setCell(int x, int y, Cell cell);
		void emptyCell(int x, int y);
		int getEvaluation();
		const std::vector<std::vector<Cell>>& getGrid() const;

	private:
		std::vector<std::vector<Cell>> _grid;
};
