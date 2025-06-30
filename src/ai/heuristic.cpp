#include "../../includes/heuristic.hpp"


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

int evaluate_player(const std::vector<std::vector<int>>& board, int player) {
    int score = 0;
    const int SIZE = board.size();
    std::vector<std::pair<int, int>> directions = {{1,0},{0,1},{1,1},{-1,1}};

    for (int y = 0; y < SIZE; ++y) {
        for (int x = 0; x < SIZE; ++x) {
            if (board[y][x] != player)
                continue;

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
    return score;
}

int count_free_threes(const std::vector<std::vector<int>>& board, int player) {
    int count = 0;
    int SIZE = board.size();
    std::vector<std::pair<int, int>> directions = {{1,0},{0,1},{1,1},{-1,1}};
    for (int y = 0; y < SIZE; ++y) {
        for (int x = 0; x < SIZE; ++x) {
            for (auto [dx, dy] : directions) {
                std::vector<int> line;
                for (int k = -1; k < 4; ++k) {
                    int nx = x + dx * k;
                    int ny = y + dy * k;
                    if (nx >= 0 && nx < SIZE && ny >= 0 && ny < SIZE)
                        line.push_back(board[ny][nx] == player ? 1 : (board[ny][nx] == 0 ? 0 : 2));
                    else
                        line.push_back(2);
                }
                if (line.size() == 5) {
                    if (line[0] == 0 && line[1] == 1 && line[2] == 1 && line[3] == 1 && line[4] == 0)
                        count++;
                }
            }
        }
    }
    return count;
}

std::set<std::pair<int, int>> detect_immediate_wins(const std::vector<std::vector<int>>& board, int player) {
    int SIZE = board.size();
    std::set<std::pair<int, int>> wins;
    std::vector<std::pair<int, int>> directions = {{1,0},{0,1},{1,1},{-1,1}};

    std::vector<std::vector<int>> patterns = {
        {0, player, player, player, player},
        {player, 0, player, player, player},
        {player, player, 0, player, player},
        {player, player, player, 0, player},
        {player, player, player, player, 0}
    };

    for (int y = 0; y < SIZE; ++y) {
        for (int x = 0; x < SIZE; ++x) {
            for (auto [dx, dy] : directions) {
                for (const auto& pat : patterns) {
                    int len = pat.size();
                    int empty_y = -1, empty_x = -1;
                    bool match = true;
                    for (int k = 0; k < len; ++k) {
                        int nx = x + dx * k;
                        int ny = y + dy * k;
                        if (nx < 0 || nx >= SIZE || ny < 0 || ny >= SIZE) {
                            match = false;
                            break;
                        }
                        int cell = board[ny][nx];
                        if (pat[k] == 0) {
                            if (cell == 0) {
                                empty_y = ny;
                                empty_x = nx;
                            } else {
                                match = false;
                                break;
                            }
                        } else if (cell != pat[k]) {
                            match = false;
                            break;
                        }
                    }
                    if (match && empty_y != -1 && empty_x != -1) {
                        wins.insert({empty_y, empty_x});
                    }
                }
            }

            if (checkCapturePair(board, x, y, player))
                wins.insert({y, x});
        }
    }
    return wins;
}

std::set<std::pair<int, int>> detect_immediate_threats(const std::vector<std::vector<int>>& board, int opponent) {
    int SIZE = board.size();
    std::set<std::pair<int, int>> threats;
    std::vector<std::pair<int, int>> directions = {{1,0},{0,1},{1,1},{-1,1}};

    std::vector<std::vector<int>> patterns = {
        {0, opponent, 0, opponent, opponent, 0},
        {0, opponent, opponent, 0, opponent, 0},
        {0, opponent, opponent, opponent, 0},
        {opponent, opponent, opponent, opponent, 0},
        {0, opponent, opponent, opponent, opponent},
        {0, opponent, opponent, opponent, opponent, 0},
        {opponent, 0, opponent, opponent, opponent},
        {opponent, opponent, 0, opponent, opponent},
        {opponent, opponent, opponent, 0, opponent}
    };

    for (int y = 0; y < SIZE; ++y) {
        for (int x = 0; x < SIZE; ++x) {
            for (auto [dx, dy] : directions) {
                for (const auto& pat : patterns) {
                    int len = pat.size();
                    std::vector<std::pair<int, int>> empty_spots;
                    bool match = true;
                    for (int k = 0; k < len; ++k) {
                        int nx = x + dx * k;
                        int ny = y + dy * k;
                        if (nx < 0 || nx >= SIZE || ny < 0 || ny >= SIZE) {
                            match = false;
                            break;
                        }
                        int cell = board[ny][nx];
                        if (pat[k] == 0) {
                            if (cell == 0)
                                empty_spots.push_back({ny, nx});
                            else {
                                match = false;
                                break;
                            }
                        } else if (pat[k] == opponent) {
                            if (cell != opponent) {
                                match = false;
                                break;
                            }
                        } else {
                            match = false;
                            break;
                        }
                    }
                    if (match && !empty_spots.empty()) {
                        for (auto pos : empty_spots)
                        threats.insert(pos);
                }
            }

            if (checkCapturePair(board, x, y, opponent))
                threats.insert({y, x});
            }
        }
    }
    return threats;
}


bool is_crossed_free_three_and_four(const std::vector<std::vector<int>>& board, int y, int x, int player) {
    if (board[y][x] != 0)
        return false;
    int SIZE = board.size();
    std::vector<std::pair<int, int>> directions = {{1,0},{0,1},{1,1},{-1,1}};
    std::vector<std::vector<int>> free_three_patterns = {
        {0, player, player, player, 0}
    };
    std::vector<std::vector<int>> four_patterns = {
        {player, player, player, player},
        {0, player, player, player, player},
        {player, player, player, player, 0},
        {0, player, player, player, player, 0}
    };

    bool found_free_three = false;
    bool found_four = false;

    for (auto [dx, dy] : directions) {
        for (int offset = -4; offset <= 0; ++offset) {
            for (const auto& pat : free_three_patterns) {
                bool match = true;
                for (int k = 0; k < 5; ++k) {
                    int nx = x + (offset + k) * dx;
                    int ny = y + (offset + k) * dy;
                    if (nx < 0 || nx >= SIZE || ny < 0 || ny >= SIZE) {
                        match = false;
                        break;
                    }
                    int cell = (nx == x && ny == y) ? player : board[ny][nx];
                    if (cell != pat[k]) {
                        match = false;
                        break;
                    }
                }
                if (match)
                    found_free_three = true;
            }
        }
        for (int offset = -5; offset <= 0; ++offset) {
            for (const auto& pat : four_patterns) {
                int len = pat.size();
                bool match = true;
                for (int k = 0; k < len; ++k) {
                    int nx = x + (offset + k) * dx;
                    int ny = y + (offset + k) * dy;
                    if (nx < 0 || nx >= SIZE || ny < 0 || ny >= SIZE) {
                        match = false;
                        break;
                    }
                    int cell = (nx == x && ny == y) ? player : board[ny][nx];
                    if (cell != pat[k]) {
                        match = false;
                        break;
                    }
                }
                if (match)
                    found_four = true;
            }
        }
    }
    return found_free_three && found_four;
}

std::set<std::pair<int, int>> detect_free_threes(const std::vector<std::vector<int>>& board, int player) {
    int SIZE = board.size();
    std::set<std::pair<int, int>> free_threes;
    std::vector<std::pair<int, int>> directions = {{1,0},{0,1},{1,1},{-1,1}};

    std::vector<int> pattern = {0, player, player, player, 0};

    for (int y = 0; y < SIZE; ++y) {
        for (int x = 0; x < SIZE; ++x) {
            for (auto [dx, dy] : directions) {
                bool match = true;
                std::vector<std::pair<int, int>> empty_spots;
                for (int k = 0; k < 5; ++k) {
                    int nx = x + dx * k;
                    int ny = y + dy * k;
                    if (nx < 0 || nx >= SIZE || ny < 0 || ny >= SIZE) {
                        match = false;
                        break;
                    }
                    int cell = board[ny][nx];
                    if (pattern[k] == 0) {
                        if (cell == 0)
                            empty_spots.push_back({ny, nx});
                        else {
                            match = false;
                            break;
                        }
                    } else if (cell != pattern[k]) {
                        match = false;
                        break;
                    }
                }
                if (match && empty_spots.size() == 2) {
                    for (auto pos : empty_spots)
                        free_threes.insert(pos);
                }
            }
        }
    }
    return free_threes;
}

std::set<std::pair<int, int>> detect_free_fours(const std::vector<std::vector<int>>& board, int player) {
    int SIZE = board.size();
    std::set<std::pair<int, int>> free_fours;
    std::vector<std::pair<int, int>> directions = {{1,0},{0,1},{1,1},{-1,1}};

    std::vector<int> pattern = {0, player, player, player, player, 0};

    for (int y = 0; y < SIZE; ++y) {
        for (int x = 0; x < SIZE; ++x) {
            for (auto [dx, dy] : directions) {
                bool match = true;
                std::vector<std::pair<int, int>> empty_spots;
                for (int k = 0; k < 6; ++k) {
                    int nx = x + dx * k;
                    int ny = y + dy * k;
                    if (nx < 0 || nx >= SIZE || ny < 0 || ny >= SIZE) {
                        match = false;
                        break;
                    }
                    int cell = board[ny][nx];
                    if (pattern[k] == 0) {
                        if (cell == 0)
                            empty_spots.push_back({ny, nx});
                        else {
                            match = false;
                            break;
                        }
                    } else if (cell != pattern[k]) {
                        match = false;
                        break;
                    }
                }
                if (match && empty_spots.size() == 2) {
                    for (auto pos : empty_spots)
                        free_fours.insert(pos);
                }
            }
        }
    }
    return free_fours;
}

std::set<std::pair<int, int>> detect_capture_moves(const std::vector<std::vector<int>>& board, int player) {
    int SIZE = board.size();
    std::set<std::pair<int, int>> capture_moves;
    int opponent = (player == 1 ? 2 : 1);
    const int directions[4][2] = {
        {1, 0},
        {0, 1},
        {1, 1},
        {1, -1}
    };

    for (int y = 0; y < SIZE; ++y) {
        for (int x = 0; x < SIZE; ++x) {
            if (board[y][x] != 0)
                continue;
            for (auto& dir : directions) {
                int nx1 = x + dir[0];
                int ny1 = y + dir[1];
                int nx2 = x + 2 * dir[0];
                int ny2 = y + 2 * dir[1];
                int nx3 = x + 3 * dir[0];
                int ny3 = y + 3 * dir[1];
                if (nx1 >= 0 && nx1 < SIZE && ny1 >= 0 && ny1 < SIZE &&
                    nx2 >= 0 && nx2 < SIZE && ny2 >= 0 && ny2 < SIZE &&
                    nx3 >= 0 && nx3 < SIZE && ny3 >= 0 && ny3 < SIZE &&
                    board[ny1][nx1] == opponent &&
                    board[ny2][nx2] == opponent &&
                    board[ny3][nx3] == player) {
                    capture_moves.insert({y, x});
                }
                
                nx1 = x - dir[0];
                ny1 = y - dir[1];
                nx2 = x - 2 * dir[0];
                ny2 = y - 2 * dir[1];
                nx3 = x - 3 * dir[0];
                ny3 = y - 3 * dir[1];
                if (nx1 >= 0 && nx1 < SIZE && ny1 >= 0 && ny1 < SIZE &&
                    nx2 >= 0 && nx2 < SIZE && ny2 >= 0 && ny2 < SIZE &&
                    nx3 >= 0 && nx3 < SIZE && ny3 >= 0 && ny3 < SIZE &&
                    board[ny1][nx1] == opponent &&
                    board[ny2][nx2] == opponent &&
                    board[ny3][nx3] == player) {
                    capture_moves.insert({y, x});
                }
            }
        }
    }
    return capture_moves;
}


int evaluate_board(const std::vector<std::vector<int>>& board, int player, int currentPlayer, int my_captures, int opp_captures) {
    int opponent = (player == 1 ? 2 : 1);
    int my_score = evaluate_player(board, player);
    int opp_score = evaluate_player(board, opponent);
    if (my_score >= 1000000)
        return 10000000;
    if (opp_score >= 1000000)
        return -10000000;

    if (my_captures >= 5)
        return 10000000;
    if (opp_captures >= 5)
        return -10000000;

    int opp_free_threes = count_free_threes(board, opponent);
    if (opp_free_threes >= 1)
        return -5000000;

    int my_free_threes = count_free_threes(board, player);
    if (my_free_threes >= 1)
        return 5000000;

    if (my_score >= 10000)
        return 500000000;
    if (opp_score>= 10000)
        return -500000000;

    if (my_score >= 10000 && currentPlayer != player)
    return -1000000;
    if (opp_score >= 1000 && currentPlayer != player)
    return -500000;

    if (currentPlayer == player) {
        return my_score - opp_score;
    }
    else {
        return my_score - (opp_score * 2);
    }
}


bool checkCapturePair(const std::vector<std::vector<int>>& board, int x, int y, int player) {
    if (board[y][x] != 0)
        return false;
    int SIZE = board.size();
    const int directions[4][2] = {
        {1, 0},
        {0, 1},
        {1, 1},
        {1, -1}
    };

    int opponent = (player == 1 ? 2 : 1);

    for (auto& dir : directions) {
        int nx1 = x + dir[0];
        int ny1 = y + dir[1];
        int nx2 = x + 2 * dir[0];
        int ny2 = y + 2 * dir[1];
        int nx3 = x + 3 * dir[0];
        int ny3 = y + 3 * dir[1];

        if (nx1 >= 0 && nx1 < SIZE && ny1 >= 0 && ny1 < SIZE &&
            nx2 >= 0 && nx2 < SIZE && ny2 >= 0 && ny2 < SIZE &&
            nx3 >= 0 && nx3 < SIZE && ny3 >= 0 && ny3 < SIZE) {
            if (board[ny1][nx1] == opponent &&
                board[ny2][nx2] == opponent &&
                board[ny3][nx3] == player) {
                return true;
            }
        }

        nx1 = x - dir[0];
        ny1 = y - dir[1];
        nx2 = x - 2 * dir[0];
        ny2 = y - 2 * dir[1];
        nx3 = x - 3 * dir[0];
        ny3 = y - 3 * dir[1];

        if (nx1 >= 0 && nx1 < SIZE && ny1 >= 0 && ny1 < SIZE &&
            nx2 >= 0 && nx2 < SIZE && ny2 >= 0 && ny2 < SIZE &&
            nx3 >= 0 && nx3 < SIZE && ny3 >= 0 && ny3 < SIZE) {
            if (board[ny1][nx1] == opponent &&
                board[ny2][nx2] == opponent &&
                board[ny3][nx3] == player) {
                return true;
            }
        }
    }
    return false;
}


bool spotFreeThreePattern(const std::vector<std::vector<int>>& board, int x, int y, int dx, int dy, int player) {
    int SIZE = board.size();
    std::string pattern;
    const std::vector<std::string> validPatterns = {
        "_OOO_", "_O_OO_", "_OO_O_",
    };

    for (int i = -4; i <= 4; ++i) {
        int nx = x + i * dx;
        int ny = y + i * dy;

        if (nx < 0 || nx >= SIZE || ny < 0 || ny >= SIZE) {
            pattern += "X";
            continue;
        }

        if (nx == x && ny == y) {
            pattern += "O";
            continue;
        }

        int cell = board[ny][nx];
        if (cell == 0)
            pattern += "_";
        else if (cell == player)
            pattern += "O";
        else
            pattern += "X";
    }

    for (const auto& p : validPatterns) {
        if (pattern.find(p) != std::string::npos)
            return true;
    }

    return false;
}

bool checkDoubleFreeThree(const std::vector<std::vector<int>>& board, int x, int y, int player) {
    int SIZE = board.size();
    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE)
        return false;

    int count = 0;
    const int directions[4][2] = {
        {1, 0},
        {0, 1},
        {1, 1},
        {1, -1}
    };

    for (auto& direction : directions) {
        if (spotFreeThreePattern(board, x, y, direction[0], direction[1], player))
            count++;
    }
    return count >= 2;
}