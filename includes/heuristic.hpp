#pragma once

#include <vector>
#include <utility>
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <string>

int pattern_score(int count);

int evaluate_player(const std::vector<std::vector<int>>& board, int player);
int evaluate_board(const std::vector<std::vector<int>>& board, int player, int currentPlayer);