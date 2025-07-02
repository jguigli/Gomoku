#pragma once
#include "../includes/minmax.hpp"
#include "../includes/heuristic.hpp"
#include "../includes/board.hpp"
#include <chrono>

class Agent
{
    public:
        Agent(int depth, Cell iaCell, Player& _blackStone, Player& _whiteStone);

        struct Result
        {
            int     y;
            int     x;
            int     score;
            int     depthReached;
            float   elapsedMs;
            bool    movePlayed;
        };

        Result play(std::vector<std::vector<Cell>>& board);
        Agent::Result playHint(std::vector<std::vector<Cell>>& board, Cell playerCell);

        void setDepth(int depth) { this->depth = depth; }
        void setCell (Cell iaCell) { this->iaCell = iaCell; }

    private:
        int depth;
        Cell iaCell;
        Player& _blackStone;
        Player& _whiteStone;
};
