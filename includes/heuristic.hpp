#pragma once

#include <vector>
#include <utility>
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <string>
#include <set>

int pattern_score(int count);

int evaluate_player(const std::vector<std::vector<int>>& board, int player);
int evaluate_board(const std::vector<std::vector<int>>& board, int player, int currentPlayer);
std::set<std::pair<int, int>> detect_immediate_wins(const std::vector<std::vector<int>>& board, int player);
std::set<std::pair<int, int>> detect_immediate_threats(const std::vector<std::vector<int>>& board, int opponent);
std::set<std::pair<int, int>> detect_free_threes(const std::vector<std::vector<int>>& board, int player);
std::set<std::pair<int, int>> detect_free_fours(const std::vector<std::vector<int>>& board, int player);