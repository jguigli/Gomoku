#include "../../includes/minmax.hpp"

std::unordered_map<std::string, int> transposition_table;

std::vector<std::pair<int, int>> generate_moves(const std::vector<std::vector<int>>& board, int player) {
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

    auto capture_moves = detect_capture_moves(board, player);
        for (const auto& pos : capture_moves)
            moves.insert(pos);

    auto free_threes = detect_free_threes(board, player);
    for (const auto& pos : free_threes)
        moves.insert(pos);

    int opponent = (player == 1 ? 2 : 1);
    auto capture_threats = detect_capture_moves(board, opponent);
    for (const auto& pos : capture_threats)
        moves.insert(pos);

    std::vector<std::pair<int, int>> filtered_moves;
    for (auto& move : moves) {
        if (!checkDoubleFreeThree(board, move.second, move.first, player) || 
            (checkDoubleFreeThree(board, move.second, move.first, player) && checkCapturePair(board, move.second, move.first, player))) {
            filtered_moves.push_back(move);
        }
    }

    if (filtered_moves.empty()) {
        return std::vector<std::pair<int, int>>(moves.begin(), moves.end());
    }
    
    return filtered_moves;
}

int move_priority(const std::vector<std::vector<int>>& board, int y, int x, int player) {
    int SIZE = board.size();
    if (y < 0 || y >= SIZE || x < 0 || x >= SIZE || board[y][x] != 0)
        return 0;

    int score = 0;
    auto tmp = board;
    tmp[y][x] = player;

    if (is_crossed_win(tmp, y, x, player))
        return 100000000;

    int eval_player = evaluate_player(tmp, player);
    if (eval_player >= 1000000)
        return 10000000;

    if (eval_player >= 100000)
        return score += 100000;

    tmp[y][x] = 0;

    tmp[y][x] = 3 - player;
    int eval_opp = evaluate_player(tmp, 3 - player);
    if (eval_opp >= 1000000)
        return score += 9000000;
    else if (eval_opp >= 10000)
        return score += 900000;

    tmp[y][x] = 0;

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
    return score;
}


std::string board_to_string(const std:: vector<std::vector<int>> & board){
    std::string result;
    for (const auto& row : board)
        for (int cell : row)
            result += std::to_string(cell);
    return result;
}

std::vector<std::pair<int, int>> simulate_capture(std::vector<std::vector<int>>& board, int y, int x, int player) {
    std::vector<std::pair<int, int>> captured_positions;
    int SIZE = board.size();
    int opponent = 3 - player;
    const int directions[4][2] = { {1,0},{0,1},{1,1},{1,-1} };
    for (auto& dir : directions) {
        int ny1 = y + dir[0], nx1 = x + dir[1];
        int ny2 = y + 2*dir[0], nx2 = x + 2*dir[1];
        int ny3 = y + 3*dir[0], nx3 = x + 3*dir[1];
        if (ny3 >= 0 && ny3 < SIZE && nx3 >= 0 && nx3 < SIZE) {
            if (board[ny1][nx1] == opponent && board[ny2][nx2] == opponent && board[ny3][nx3] == player) {
                captured_positions.emplace_back(ny1, nx1);
                captured_positions.emplace_back(ny2, nx2);
                board[ny1][nx1] = 0;
                board[ny2][nx2] = 0;
            }
        }
    }
    return captured_positions;
}

int minimax(std::vector<std::vector<int>>& board, int depth, int alpha, int beta, bool maximizingPlayer, int player, int currentPlayer, std::chrono::high_resolution_clock::time_point startTime, float maxTimeMs, bool& timeout, int my_captures, int opp_captures) {
    auto now = std::chrono::high_resolution_clock::now();
    float elapsed = std::chrono::duration<float, std::milli>(now - startTime).count();
    if (elapsed > maxTimeMs) {
        timeout = true;        
        return 0;
    }
    auto wins = detect_immediate_wins(board, currentPlayer);
    if (!wins.empty())
        return maximizingPlayer ? 10000000 : -10000000;

    int opponent = (currentPlayer == 1 ? 2 : 1);
    auto threats = detect_immediate_threats(board, opponent);
    if (!threats.empty())
        return maximizingPlayer ? -9000000 : 9000000;

    auto free_fours = detect_free_fours(board, currentPlayer);
    if (!free_fours.empty())
        return maximizingPlayer ? 8000000 : -8000000;

    auto free_threes = detect_free_threes(board, currentPlayer);
    if (!free_threes.empty())
        return maximizingPlayer ? 5000000 : -5000000;

    std::string hash = board_to_string(board) + std::to_string(currentPlayer);
    auto it = transposition_table.find(hash);
    if (it != transposition_table.end()) {
        return it->second;
    }
    auto moves = generate_moves(board, player);
    if (moves.size() > 7)
        moves.resize(7);
    
    if (depth == 0 || generate_moves(board, player).empty()) {
        int eval = evaluate_board(board, player, currentPlayer, my_captures, opp_captures);
        transposition_table[hash] = eval;
        return eval;
    }

    std::sort(moves.begin(), moves.end(), [&](const std::pair<int, int>& a, const std::pair<int, int>& b) {
        return move_priority(board, a.first, a.second, currentPlayer) > move_priority(board, b.first, b.second, currentPlayer);
    });

    if (maximizingPlayer) {
        int maxEval = std::numeric_limits<int>::min();
        for (auto [y, x] : moves) {
            board[y][x] = currentPlayer;
            auto captured_positions = simulate_capture(board, y, x, currentPlayer);
            int captured = captured_positions.size() / 2;
            int next_my_captures = my_captures;
            int next_opp_captures = opp_captures;
            if (currentPlayer == player)
                next_my_captures += captured;
            else
                next_opp_captures += captured;
            int newCurrentPlayer = 3 - currentPlayer;
            int nextDepth = depth - 1;
            int eval = minimax(board, nextDepth, alpha, beta, false, player, newCurrentPlayer,  startTime, maxTimeMs, timeout, next_my_captures, next_opp_captures);
            board[y][x] = 0;
            for (auto& pos : captured_positions)
                board[pos.first][pos.second] = 3 - currentPlayer;
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
            auto captured_positions = simulate_capture(board, y, x, currentPlayer);
            int captured = captured_positions.size() / 2;
            int next_my_captures = my_captures;
            int next_opp_captures = opp_captures;
            if (currentPlayer == player)
                next_my_captures += captured;
            else
                next_opp_captures += captured;
            int nextDepth = depth - 1;
            int newCurrentPlayer = 3 - currentPlayer;
            int eval = minimax(board, nextDepth, alpha, beta, true, player, newCurrentPlayer, startTime, maxTimeMs, timeout, next_my_captures, next_opp_captures);
            board[y][x] = 0;
            for (auto& pos : captured_positions)
                board[pos.first][pos.second] = 3 - currentPlayer;
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