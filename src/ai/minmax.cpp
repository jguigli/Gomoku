#include "../../includes/minmax.hpp"

std::unordered_map<std::string, int> transposition_table;

std::vector<std::pair<int, int>> generate_moves(const std::vector<std::vector<int>>& board, int player) {
    int SIZE = board.size();
    std::set<std::pair<int, int>> moves;
    std::vector<std::pair<int, int>> stones;

    for (int y = 0; y < SIZE; ++y)
        for (int x = 0; x < SIZE; ++x)
            if (board[y][x] != 0)
                stones.emplace_back(y, x);

    if (stones.empty()) {
        moves.emplace(SIZE/2, SIZE/2);
    } else {
        int min_y = SIZE, max_y = 0, min_x = SIZE, max_x = 0;
        for (auto [sy, sx] : stones) {
            min_y = std::min(min_y, sy);
            max_y = std::max(max_y, sy);
            min_x = std::min(min_x, sx);
            max_x = std::max(max_x, sx);
        int margin = (stones.size() < 6) ? 2 : 1;
        for (int y = std::max(0, min_y - margin); y <= std::min(SIZE - 1, max_y + margin); ++y)
            for (int x = std::max(0, min_x - margin); x <= std::min(SIZE - 1, max_x + margin); ++x)
                if (board[y][x] == 0)
                    moves.emplace(y, x);
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

int move_priority(const std::vector<std::vector<int>>& board, int y, int x, int player, int my_captures, int opp_captures) {
    int SIZE = board.size();
    if (y < 0 || y >= SIZE || x < 0 || x >= SIZE || board[y][x] != 0)
        return 0;

    int score = 0;
    auto tmp = board;
    tmp[y][x] = player;

    if (is_crossed_free_three_and_four(tmp, y, x, player))
        return 100000000;

    int eval_player = evaluate_player(tmp, player);
    if (eval_player >= 1000000)
        return 10000000;

    if (eval_player >= 100000)
        return score += 100000;

    auto captured_positions = simulate_capture(tmp, y, x, player);
    int captured = captured_positions.size() / 2;
    if (my_captures + captured >= 5)
        return 90000000;

    tmp[y][x] = 0;

    tmp[y][x] = 3 - player;
    int eval_opp = evaluate_player(tmp, 3 - player);
    if (eval_opp >= 1000000)
        return score += 9000000;
    else if (eval_opp >= 10000)
        return score += 900000;

    auto opp_captured_positions = simulate_capture(tmp, y, x, 3 - player);
    int opp_captured = opp_captured_positions.size() / 2;
    if (opp_captures + opp_captured >= 5)
        score += 8000000;

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
                captured_positions.emplace_back(ny1, nx1);
                captured_positions.emplace_back(ny2, nx2);
                board[ny1][nx1] = 0;
                board[ny2][nx2] = 0;
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
                captured_positions.emplace_back(ny1, nx1);
                captured_positions.emplace_back(ny2, nx2);
                board[ny1][nx1] = 0;
                board[ny2][nx2] = 0;
            }
        }
    }
    return captured_positions;
}

int moves_to_keep(int depth) {
    if (depth >= 3) return 4;
    if (depth == 2) return 6;
    return 8;
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

    std::string hash = board_to_string(board) + std::to_string(currentPlayer) + std::to_string(my_captures) + std::to_string(opp_captures);
    auto it = transposition_table.find(hash);
    if (it != transposition_table.end()) {
        return it->second;
    }
    auto moves = generate_moves(board, player);
    
    if (depth == 0 || moves.empty()) {
        int eval = evaluate_board(board, player, currentPlayer, my_captures, opp_captures);
        transposition_table[hash] = eval;
        return eval;
    }

    std::vector<std::tuple<int, int, int>> move_scores;
    for (auto& move : moves) {
        int score = move_priority(board, move.first, move.second, currentPlayer, my_captures, opp_captures);
        move_scores.emplace_back(move.first, move.second, score);
    }

    std::sort(move_scores.begin(), move_scores.end(),
        [maximizingPlayer](const auto& a, const auto& b) {
            return maximizingPlayer ? std::get<2>(a) > std::get<2>(b)
                                : std::get<2>(a) < std::get<2>(b);
        });

    moves.clear();
    int keep = moves_to_keep(depth);
    for (size_t i = 0; i < move_scores.size() && i < (size_t)keep; ++i)
        moves.emplace_back(std::get<0>(move_scores[i]), std::get<1>(move_scores[i]));

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