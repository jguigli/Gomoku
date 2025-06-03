#pragma once
#include <vector>
#include <utility>

std::vector<std::pair<int, int>> generate_moves(const std::vector<std::vector<int>>& board);
int minimax(std::vector<std::vector<int>>& board, int depth, int alpha, int beta, bool maximizingPlayer, int player);