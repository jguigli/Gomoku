#include <vector>
#include <utility>
#include <algorithm>
#include <iostream>

int pattern_score(int count, int openEnds) {
    switch (count) {
        case 2:
            switch (openEnds) {
                case 2: return 100;
                case 1: return 10;
                default: return 0;
            }
        case 3:
            switch (openEnds) {
                case 2: return 1000;
                case 1: return 100;
                default: return 0;
            }
        case 4:
            switch (openEnds) {
                case 2: return 10000;
                case 1: return 1000;
                default: return 0;
            }
        case 5:
            return 100000;
        default:
            return 0;
    }
}

bool has_gap_pattern(const std::vector<int> & line, int player, int length) {
    int count = 0;
    int gap_pos = -1;
    if (length < 3 || (int)line.size() < length)
        return false;
    for (int i =0; i < length; ++i) {
        if (line[i] == player)
            count++;
        else if (line[i] == 0) {
            if (gap_pos == -1)
                gap_pos = i;
            else
                return false;
        } else
            return false;
    }
    return (count == length - 1 && gap_pos > 0 && gap_pos < length - 1);
}

int detect_gap_patterns(const std::vector<std::vector<int>>& board, int player, int length, int dx, int dy, int bonus_gap) {
    int score = 0;
    int SIZE = board.size();

    for (int y = 0; y < SIZE; ++y) {
        for (int x = 0; x < SIZE; ++x) {
            std::vector<int> line;
            int valid = 1;
            for (int k = 0; k < length; ++k) {
                int nx = x + k * dx;
                int ny = y + k * dy;
                if (nx >= 0 && ny >= 0 && nx < SIZE && ny < SIZE)
                    line.push_back(board[ny][nx]);
                else
                    valid = 0;
            }
            if (!valid)
                continue;

            int beforeX = x - dx;
            int beforeY = y - dy;
            int afterX = x + dx * length;
            int afterY = y + dy * length;
            int openEnds = 0;
            if (beforeX >= 0 && beforeY >= 0 && beforeX < SIZE && beforeY < SIZE && board[beforeY][beforeX] == 0)
                openEnds++;
            if (afterX >= 0 && afterY >= 0 && afterX < SIZE && afterY < SIZE && board[afterY][afterX] == 0)
                openEnds++;
            if (openEnds != 2)
                continue;

            if (has_gap_pattern(line, player, length)) {
                score += bonus_gap;
            }
        }
    }
    return score;
}

int evaluate_board(const std::vector<std::vector<int>>& board, int player) {
    int score = 0;
    const int SIZE = board.size();

    std::vector<std::pair<int, int>> directions = {
        {1, 0}, {0, 1}, {1, 1}, {1, -1}
    };

    for (int y = 0; y < SIZE; ++y) {
        for (int x = 0; x < SIZE; ++x) {
            if (board[y][x] != player)
                continue;
            for (auto [dx, dy] : directions) {
                int prevX = x - dx;
                int prevY = y - dy;
                if (prevX >= 0 && prevY >= 0 && prevX < SIZE && prevY < SIZE && board[prevY][prevX] == player)
                    continue;

                int count = 0;
                int nx = x, ny = y;
                while (nx >= 0 && ny >= 0 && nx < SIZE && ny < SIZE && board[ny][nx] == player) {
                    count++;
                    nx += dx;
                    ny += dy;
                }
                if (count < 2)
                    continue;

                int openEnds = 0;
                if (prevX >= 0 && prevY >= 0 && prevX < SIZE && prevY < SIZE && board[prevY][prevX] == 0)
                    openEnds++;
                if (nx >= 0 && ny >= 0 && nx < SIZE && ny < SIZE && board[ny][nx] == 0)
                    openEnds++;

                score += pattern_score(count, openEnds);
            }
        }
    }
    for (auto [dx, dy] : directions) {
        score += detect_gap_patterns(board, player, 3, dx, dy, 1000);
        score += detect_gap_patterns(board, player, 4, dx, dy, 10000);
        score += detect_gap_patterns(board, player, 5, dx, dy, 100000);
    }
    return score;
}