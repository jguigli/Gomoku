#pragma once
#include "heuristic.hpp"
#include <iostream>
#include <algorithm>
#include <vector>
#include <limits>
#include <unordered_map>
#include <string>
#include <chrono>

extern std::unordered_map<std::string, int> transposition_table;
std::vector<std::pair<int, int>> simulate_capture(std::vector<std::vector<int>>& board, int y, int x, int player);
std::vector<std::pair<int, int>> generate_moves(const std::vector<std::vector<int>>& board, int player);
int move_priority(const std::vector<std::vector<int>>& board, int y, int x, int player, int my_captures, int opp_captures);
int minimax(std::vector<std::vector<int>>& board, int depth, int alpha, int beta, bool maximizingPlayer, int player, int currentPlayer, std::chrono::high_resolution_clock::time_point startTime, float maxTimeMs, bool& timeout, int my_captures, int opp_captures);