#pragma once
#include "board.hpp"

class Game
{
	private:
		Board board;
		bool blackTurn;
		Rectangle resetButton;

	public:
		Game();
		void update();
		void render();
		void reset();
		bool checkFiveStoneAligned();
		bool checkCapturePair();
};
