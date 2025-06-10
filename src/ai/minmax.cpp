#include "../includes/minmax.hpp"

static std::unordered_map<std::string, int> transposition_table;

std::vector<std::pair<int, int>> generate_moves(const std::vector<std::vector<int>>& board) {
    int SIZE = board.size();
    int margin = 1;
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

std::vector<std::pair<int, int>> find_blocking_moves(const std::vector<std::vector<int>>& board, int opponent) {
    int SIZE = board.size();
    std::vector<std::pair<int, int>> blocking_moves;
    for (int y = 0; y < SIZE; ++y) {
        for (int x = 0; x < SIZE; ++x) {
            if (board[y][x] != 0)
                continue;
            auto tmp = board;
            tmp[y][x] = opponent;
            if (evaluate_player(tmp, opponent) >= 100000)
                blocking_moves.emplace_back(y, x);
        }
    }
    return blocking_moves;
}

int move_priority(const std::vector<std::vector<int>>& board, int y, int x, int player) {
    int SIZE = board.size();
    if (y < 0 || y >= SIZE || x < 0 || x >= SIZE || board[y][x] != 0)
        return 0;

    int score = 0;
    for (int dy = -1; dy <= 1; ++dy)
        for (int dx = -1; dx <= 1; ++dx)
            if (dy != 0 || dx != 0) {
                int ny = y + dy, nx = x + dx;
                if (ny >= 0 && ny < SIZE && nx >= 0 && nx < SIZE) {
                    if (board[ny][nx] == player)
                        score += 10;
                    else if (board[ny][nx] != 0)
                        score -= 5;
                }
            }
    return score * score;
}


std::string board_to_string(const std:: vector<std::vector<int>> & board){
    std::string result;
    for (const auto& row : board)
        for (int cell : row)
            result += std::to_string(cell);
    return result;
}

int minimax(std::vector<std::vector<int>>& board, int depth, int alpha, int beta, bool maximizingPlayer, int player, int currentPlayer, std::chrono::high_resolution_clock::time_point startTime, float maxTimeMs, bool& timeout) {
    auto now = std::chrono::high_resolution_clock::now();
    float elapsed = std::chrono::duration<float, std::milli>(now - startTime).count();
    if (elapsed > maxTimeMs) {
        timeout = true;
        return 0;
    }
    
    std::string hash = board_to_string(board);
    auto it = transposition_table.find(hash);
    if (it != transposition_table.end()) {
        return it->second;
    }
    
    if (depth == 0 || generate_moves(board).empty()) {
        int eval = evaluate_board(board, player);
        transposition_table[hash] = eval;
        return eval;
    }

    auto moves = generate_moves(board);
    std::sort(moves.begin(), moves.end(), [&](const std::pair<int, int>& a, const std::pair<int, int>& b) {
        return move_priority(board, a.first, a.second, currentPlayer) > move_priority(board, b.first, b.second, currentPlayer);
    });

    if (maximizingPlayer) {
        int maxEval = std::numeric_limits<int>::min();
        for (auto [y, x] : moves) {
            board[y][x] = currentPlayer;
            int eval = minimax(board, depth - 1, alpha, beta, false, player, 3 - currentPlayer,  startTime, maxTimeMs, timeout);
            board[y][x] = 0;
            if (timeout) return maxEval;
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha)
                break;
        }
        transposition_table[hash] = maxEval;
        return maxEval;
    } else {
        int minEval = std::numeric_limits<int>::max();
        for (auto [y, x] : moves) {
            board[y][x] = currentPlayer;
            int eval = minimax(board, depth - 1, alpha, beta, true, player, 3 - currentPlayer, startTime, maxTimeMs, timeout);
            board[y][x] = 0;
            if (timeout) return minEval;
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha)
                break;
        }
        transposition_table[hash] = minEval;
        return minEval;
    }
}