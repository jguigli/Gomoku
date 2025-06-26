#pragma once
#include "board.hpp"
#include "config.hpp"
#include "player.hpp"
#include "agent.hpp"
#include "menu.hpp"
#include <chrono>
#include <vector>
#include <cmath>

struct Move {
	int x;
	int y;
	Cell stone;
	int moveNumber;
	std::vector<std::pair<int, int>> capturedStones; // Store captured stone positions
};

class Game
{
	public:
		Game();
		Game(GameMode gameMode, Difficulty difficulty);
		~Game();
		void update();
		void render();
		void reset();
		void setGameMode(GameMode gameMode, Difficulty difficulty);
		bool checkFiveStonesAligned(int x, int y, Player stone);
		bool checkCapturePair(int x, int y, Player& stone);
		bool checkDoubleFreeThree(int x, int y, Player stone);
		bool spotFreeThreePattern(int x, int y, int dx, int dy, Player stone);
		bool handlePlayerMove(int x, int y, Player currentPlayer);
		void displayInfo();

	private:
		Board _board;
		Board _reviewBoard; // Separate board for review mode
		bool _blackTurn;
		Player _blackStone;
		Player _whiteStone;
		Rectangle _resetButton;
		Rectangle _menuButton;
		Rectangle _backwardButton;
		Rectangle _forwardButton;
		Agent* _agent;
		bool _agentActive;
		GameMode _gameMode;
		Difficulty _difficulty;
		std::chrono::time_point<std::chrono::high_resolution_clock> _turnStartTime;
		float _elapsedMs = 0.0f;
		float _iaElapsedMs = 0.0f;
		bool _backToMenu;
		bool _gameOver;
		
		// Move history
		std::vector<Move> _moveHistory;
		int _currentMoveIndex;
		bool _reviewMode;
		
		void addMoveToHistory(int x, int y, Cell stone, const std::vector<std::pair<int, int>>& capturedStones = {});
		void navigateToMove(int moveIndex);
		void drawMoveHighlight();
		void enterReviewMode();
		void exitReviewMode();
		
	public:
		bool shouldReturnToMenu() const { return _backToMenu; }
		void resetMenuFlag() { _backToMenu = false; }
};
