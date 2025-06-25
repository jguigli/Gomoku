#pragma once
#include "board.hpp"
#include "config.hpp"
#include "player.hpp"
#include "agent.hpp"
#include <chrono>

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
		bool spotFreeThreePattern(int x, int y, int dx, int dy, Player stone);
		bool handlePlayerMove(int x, int y, Player currentPlayer);

	private:
		Board _board;
		bool _blackTurn;
		Player _blackStone;
		Player _whiteStone;
		Rectangle _resetButton;
		Agent* _agent;
		bool _agentActive;
		std::chrono::time_point<std::chrono::high_resolution_clock> _turnStartTime;
		float _elapsedMs = 0.0f;
		float _iaElapsedMs = 0.0f;
};
