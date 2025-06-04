#pragma once
#include "board.hpp"
#include "config.hpp"
#include "player.hpp"

class Game
{
	public:
		Game();
		~Game();
		void update();
		void render();
		void reset();
		bool checkFiveStonesAligned(int x, int y, Player stone);
		bool checkCapturePair(int x, int y, Player stone);

	private:
		Board _board;
		bool _blackTurn;
		Player _blackStone;
		Player _whiteStone;
		Rectangle _resetButton;
};
