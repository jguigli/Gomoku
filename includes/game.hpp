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
		bool checkDoubleFreeThree(int x, int y, Player stone);

	private:
		Board _board;
		bool _blackTurn;
		Player _blackStone;
		Player _whiteStone;
		Rectangle _resetButton;

		const int directions[4][2] = {
			{1, 0},   // horizontal →
			{0, 1},   // vertical ↓
			{1, 1},   // diagonal ↘
			{1, -1}   // diagonal ↗
		};

		bool spotFreeThreePattern(int x, int y, int dx, int dy, Player stone);
};
