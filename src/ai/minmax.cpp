#include "../includes/minmax.hpp"
#include "../includes/heuristic.hpp"
#include <iostream>
#include <algorithm>
#include <bits/stdc++.h>
#include <vector>
#include <limits>

// int minimax(int depth, int nodeIndex, bool isMax, int scores[], int h)
// {
//     if (depth == h)
//         return scores[nodeIndex];
//     if (isMax)
//         return std::max(minimax(depth+1, nodeIndex*2, false, scores, h),
//                    minimax(depth+1, nodeIndex*2 + 1, false, scores, h));
//     else
//         return std::min(minimax(depth+1, nodeIndex*2, true, scores, h),
//                    minimax(depth+1, nodeIndex*2 + 1, true, scores, h));
// }

// int log2(int n)
// {
//     return (n==1 ? 0 : 1 + log2(n/2));
// }

// int main()
// {
//     int scores[] = {3, 5, 2, 9, 12, 5, 23, 23};
//     int n = sizeof(scores)/sizeof(scores[0]);
//     int h = log2(n);
//     int res = minimax(0,0,true,scores,h);
//     std::cout << "The optimal value is : " << res << std::endl;
//     return 0;
// }

std::vector<std::pair<int, int>> generate_moves(const std::vector<std::vector<int>>& board) {
    int SIZE = board.size();
    int margin = 2;
    std::set<std::pair<int, int>> moves;
    std::vector<std::pair<int, int>> stones;

    for (int y = 0; y < SIZE; ++y)
        for (int x = 0; x < SIZE; ++x)
            if (board[y][x] != 0)
                stones.emplace_back(y, x);

    if (stones.empty()) {
        moves.emplace(SIZE/2, SIZE/2);
    } else {
        for (auto [sy, sx] : stones) {
            for (int dy = -margin; dy <= margin; ++dy) {
                for (int dx = -margin; dx <= margin; ++dx) {
                    int ny = sy + dy;
                    int nx = sx + dx;
                    if (ny >= 0 && ny < SIZE && nx >= 0 && nx < SIZE && board[ny][nx] == 0)
                        moves.emplace(ny, nx);
                }
            }
        }
    }
    return std::vector<std::pair<int, int>>(moves.begin(), moves.end());
}

int minimax(std::vector<std::vector<int>>& board, int depth, int alpha, int beta, bool maximizingPlayer, int player) {
    if (depth == 0 || generate_moves(board).empty()) {
        return evaluate_board(board, player);
    }

    if (maximizingPlayer) {
        int maxEval = std::numeric_limits<int>::min();
        for (auto [y, x] : generate_moves(board)) {
            board[y][x] = player;
            int eval = minimax(board, depth - 1, alpha, beta, false, player == 1 ? 2 : 1);
            board[y][x] = 0;
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha)
                break;
        }
        return maxEval;
    } else {
        int minEval = std::numeric_limits<int>::max();
        for (auto [y, x] : generate_moves(board)) {
            board[y][x] = player;
            int eval = minimax(board, depth - 1, alpha, beta, true, player == 1 ? 2 : 1);
            board[y][x] = 0;
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha)
                break;
        }
        return minEval;
    }
}