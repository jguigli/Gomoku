#include "../includes/heuristic.hpp"


static std::unordered_map<std::string, int> pattern_table = {
    {"11111", 1000000},
    {"22222", 1000000},

    {"01111", 10000},
    {"10111", 10000},
    {"11011", 10000},
    {"11101", 10000},
    {"11110", 10000},

    {"02222", 10000},
    {"20222", 10000},
    {"22022", 10000},
    {"22202", 10000},
    {"22220", 10000},

    {"01110", 1000},
    {"01011", 1000},
    {"01101", 1000},
    {"10011", 1000},
    {"11001", 1000},
    {"10101", 1000},

    {"02220", 1000},
    {"02022", 1000},
    {"02202", 1000},
    {"20022", 1000},
    {"22002", 1000},
    {"20202", 1000},

    {"00110", 100},
    {"01010", 100},
    {"01100", 100},
    {"10010", 100},
    {"11000", 100},
    {"01001", 100},

    {"00220", 100},
    {"02020", 100},
    {"02200", 100},
    {"20020", 100},
    {"22000", 100},
    {"02002", 100},

    {"00010", 10},
    {"00100", 10},
    {"01000", 10},
    {"10000", 10},

    {"00020", 10},
    {"00200", 10},
    {"02000", 10},
    {"20000", 10},

    {"21111", 500},
    {"12111", 500},
    {"11211", 500},
    {"11121", 500},
    {"11112", 500},

    {"12222", 500},
    {"21222", 500},
    {"22122", 500},
    {"22212", 500},
    {"22221", 500},

    {"21110", 50},
    {"21011", 50},
    {"21101", 50},
    {"20111", 50},
    {"11012", 50},
    {"10112", 50},
    {"10121", 50},
    {"11210", 50},

    {"12220", 50},
    {"12022", 50},
    {"12202", 50},
    {"10222", 50},
    {"22021", 50},
    {"20221", 50},
    {"20212", 50},
    {"22120", 50},

    {"21010", 5},
    {"20101", 5},
    {"11020", 5},
    {"12010", 5},
    {"10120", 5},
    {"20110", 5},

    {"12020", 5},
    {"20102", 5},
    {"22010", 5},
    {"10220", 5},
    {"20210", 5},
    {"12002", 5}
};

std::string line_to_pattern(const std::vector<int>& line, int start) {
    std::string s;
    for (int i = 0; i < 5; ++i)
        s += std::to_string(line[start + i]);
    return s;
}

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
                case 2: return 10000;
                case 1: return 100;
                default: return 0;
            }
        case 4:
            switch (openEnds) {
                case 2: return 10000;
                case 1: return 10000;
                default: return 0;
            }
        case 5:
            return 1000000;
        default:
            return 0;
    }
}

bool has_gap_pattern(const std::vector<int> & line, int player, int length) {
    int count = 0;
    int gap_pos = -1;
    if (length < 3 || (int)line.size() < length)
        return false;
    for (int i = 0; i < length; ++i) {
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

int evaluate_player(const std::vector<std::vector<int>>& board, int player) {
    int score = 0;
    const int SIZE = board.size();
    std::vector<std::pair<int, int>> directions = {{1,0},{0,1},{1,1},{-1,1}};

    for (int y = 0; y < SIZE; ++y) {
        for (int x = 0; x < SIZE; ++x) {
            if (board[y][x] != player)
                continue;
            // for (auto [dx, dy] : directions) {
            //     int prevX = x - dx;
            //     int prevY = y - dy;
            //     if (prevX >= 0 && prevY >= 0 && prevX < SIZE && prevY < SIZE && board[prevY][prevX] == player)
            //         continue;

            //     int count = 0;
            //     int nx = x, ny = y;
            //     while (nx >= 0 && ny >= 0 && nx < SIZE && ny < SIZE && board[ny][nx] == player) {
            //         count++;
            //         nx += dx;
            //         ny += dy;
            //     }
            //     if (count < 2)
            //         continue;

            //     int openEnds = 0;
            //     if (prevX >= 0 && prevY >= 0 && prevX < SIZE && prevY < SIZE && board[prevY][prevX] == 0)
            //         openEnds++;
            //     if (nx >= 0 && ny >= 0 && nx < SIZE && ny < SIZE && board[ny][nx] == 0)
            //         openEnds++;

            //     score += pattern_score(count, openEnds);
            for (auto [dx, dy] : directions) {
                std::vector<int> line;
                for (int k = 0; k < 5; ++k) {
                    int nx = x + dx * k;
                    int ny = y + dy * k;
                    if (nx >= 0 && nx < SIZE && ny >= 0 && ny < SIZE)
                        line.push_back(board[ny][nx] == player ? 1 : (board[ny][nx] == 0 ? 0 : 2));
                    else
                        break;
                }
                if (line.size() == 5) {
                    std::string pattern;
                    for (int v : line) pattern += std::to_string(v);
                    auto it = pattern_table.find(pattern);
                    if (it != pattern_table.end())
                        score += it->second;
                }
            }
        }
    }
    for (auto [dx, dy] : directions) {
        score += detect_gap_patterns(board, player, 3, dx, dy, 800);
        score += detect_gap_patterns(board, player, 4, dx, dy, 8000);
        score += detect_gap_patterns(board, player, 5, dx, dy, 1000000);
    }
    return score;
}

int evaluate_board(const std::vector<std::vector<int>>& board, int player) {
    int opponent = (player == 1 ? 2 : 1);
    int my_score = evaluate_player(board, player);
    int opp_score = evaluate_player(board, opponent);
    return my_score - (int)(opp_score * 2);
}