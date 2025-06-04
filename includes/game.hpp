#pragma once
#include "board.hpp"
#include "config.hpp"

class Game
{
	public:
		Game();
		~Game();
		void update();
		void render();
		void reset();
		bool checkFiveStoneAligned();
		bool checkCapturePair();

	private:
		Board board;
		bool blackTurn;
		Rectangle resetButton;
};
