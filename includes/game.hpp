#pragma once
#include "board.hpp"
#include "config.hpp"
#include "player.hpp"
#include "ai.hpp"
#include <string>

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
		void toggleAIMode() { _aiMode = !_aiMode; }
		void setAIDifficulty(int depth) { if (_ai) _ai->setMaxDepth(depth); }

	private:
		Board _board;
		bool _blackTurn;
		Player _blackStone;
		Player _whiteStone;
		Rectangle _resetButton;
		Rectangle _aiToggleButton;
		Rectangle _difficultyButton;
		AI* _ai;
		bool _aiMode;
		bool _aiTurn;
		bool _gameOver;
		std::string _gameStatus;

		const int directions[4][2] = {
			{1, 0},   // horizontal →
			{0, 1},   // vertical ↓
			{1, 1},   // diagonal ↘
			{1, -1}   // diagonal ↗
		};

		bool spotFreeThreePattern(int x, int y, int dx, int dy, Player stone);
		void handlePlayerMove(int x, int y);
		void handleAIMove();
		void checkGameEnd(int x, int y, Player currentPlayer);
};
