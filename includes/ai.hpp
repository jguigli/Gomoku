#pragma once
#include "board.hpp"
#include "player.hpp"
#include "config.hpp"
#include <vector>
#include <chrono>
#include <climits>
#include <unordered_map>

struct Move {
    int x, y;
    int score;
    
    Move() : x(-1), y(-1), score(0) {}
    Move(int x, int y, int score = 0) : x(x), y(y), score(score) {}
};

class AI
{
	public:
		AI(Cell aiColor, int maxDepth = 4);
		~AI();
		
		Move getBestMove(Board& board, Player& aiPlayer, Player& opponent);
		void setMaxDepth(int depth) { _maxDepth = depth; }
		int getMaxDepth() const { return _maxDepth; }
		double getLastMoveTime() const { return _lastMoveTime; }
		void setTimeLimit(int milliseconds) { _timeLimit = milliseconds; }

	private:
		Cell _aiColor;
		Cell _opponentColor;
		int _maxDepth;
		double _lastMoveTime;
		int _timeLimit; // Time limit in milliseconds
		std::chrono::high_resolution_clock::time_point _searchStartTime;
		
		// Minimax with alpha-beta pruning and time cutoff
		int minimax(Board& board, Player& aiPlayer, Player& opponent, int depth, 
					int alpha, int beta, bool isMaximizing, int lastX = -1, int lastY = -1);
		
		// Iterative deepening search
		Move iterativeDeepening(Board& board, Player& aiPlayer, Player& opponent);
		
		// Heuristic evaluation functions (simplified)
		int evaluateBoard(Board& board, Player& aiPlayer, Player& opponent);
		int evaluatePosition(Board& board, int x, int y, Cell color);
		int evaluatePattern(Board& board, int x, int y, int dx, int dy, Cell color);
		int countConsecutive(Board& board, int x, int y, int dx, int dy, Cell color);
		
		// Game state evaluation
		bool checkWinCondition(Board& board, int x, int y, Cell color);
		bool isTimeUp() const;
		
		// Optimized move generation
		std::vector<Move> generatePossibleMoves(Board& board, bool limitMoves = true);
		std::vector<Move> generateThreatMoves(Board& board);
		void orderMoves(std::vector<Move>& moves, Board& board);
		bool isValidMove(Board& board, int x, int y);
		
		// Quick evaluation for move ordering
		int quickEvaluateMove(Board& board, int x, int y, Cell color);
		
		// Helper functions
		bool isInsideBounds(int x, int y) const;
		Cell getOppositeColor(Cell color) const;
		int getManhattanDistance(int x1, int y1, int x2, int y2) const;
		
		// Heuristic weights (reduced complexity)
		static constexpr int WIN_SCORE = 1000000;
		static constexpr int FOUR_SCORE = 50000;
		static constexpr int THREE_OPEN_SCORE = 5000;
		static constexpr int THREE_HALF_SCORE = 1000;
		static constexpr int TWO_OPEN_SCORE = 500;
		static constexpr int TWO_HALF_SCORE = 100;
		static constexpr int ONE_SCORE = 10;
		static constexpr int CENTER_SCORE = 20;
		static constexpr int MAX_CANDIDATE_MOVES = 15; // Limit moves for performance
};