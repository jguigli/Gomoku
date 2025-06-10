#pragma once
#include "heuristic.hpp"
#include <iostream>
#include <algorithm>
#include <bits/stdc++.h>
#include <vector>
#include <limits>
#include <unordered_map>
#include <string>
#include <chrono>

std::vector<std::pair<int, int>> generate_moves(const std::vector<std::vector<int>>& board);
// std::vector<std::pair<int, int>> find_blocking_moves(const std::vector<std::vector<int>>& board, int opponent);
int minimax(std::vector<std::vector<int>>& board, int depth, int alpha, int beta, bool maximizingPlayer, int player, int currentPlayer, std::chrono::high_resolution_clock::time_point startTime, float maxTimeMs, bool& timeout);