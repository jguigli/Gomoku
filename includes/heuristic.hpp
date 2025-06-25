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
bool is_crossed_threat(const std::vector<std::vector<int>>& board, int y, int x, int player);
bool is_crossed_win(const std::vector<std::vector<int>>& board, int y, int x, int player);

bool checkCapturePair(const std::vector<std::vector<int>>& board, int x, int y, int player);
bool spotFreeThreePattern(const std::vector<std::vector<int>>& board, int x, int y, int dx, int dy, int player);
bool checkDoubleFreeThree(const std::vector<std::vector<int>>& board, int x, int y, int player);