#pragma once
#include "heuristic.hpp"
#include <iostream>
#include <algorithm>
#include <bits/stdc++.h>
#include <vector>
#include <limits>

std::vector<std::pair<int, int>> generate_moves(const std::vector<std::vector<int>>& board);
int minimax(std::vector<std::vector<int>>& board, int depth, int alpha, int beta, bool maximizingPlayer, int player, int currentPlayer);