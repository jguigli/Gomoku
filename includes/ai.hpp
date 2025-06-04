#pragma once
#include "../includes/ai.hpp"
#include "../includes/minmax.hpp"
#include "../includes/heuristic.hpp"
#include "../includes/board.hpp"
#include <chrono>

struct AIMoveResult {
    int y;
    int x;
    int score;
    float elapsedMs;
    bool movePlayed;
};

AIMoveResult play_ai_turn(std::vector<std::vector<Cell>>& board, int profondeur, Cell iaCell);