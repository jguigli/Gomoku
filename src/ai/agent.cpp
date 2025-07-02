#include "../../includes/agent.hpp"

Agent::Agent(int depth, Cell iaCell, Player& _blackStone, Player& _whiteStone) : depth(depth), iaCell(iaCell), _blackStone(_blackStone), _whiteStone(_whiteStone) {}

Agent::Result Agent::play(std::vector<std::vector<Cell>>& board) {
    extern std::unordered_map<std::string, int> transposition_table;

    auto turnStart = std::chrono::high_resolution_clock::now();
    float maxTimeMs = 500.0f;
    bool timeout = false;

    int gridSize = board.size();
    std::vector<std::vector<int>> intBoard(gridSize, std::vector<int>(gridSize, 0));
    for (int y = 0; y < gridSize; ++y)
        for (int x = 0; x < gridSize; ++x)
            intBoard[y][x] = board[y][x];

    int bestY = -1, bestX = -1, bestScore = std::numeric_limits<int>::min();
    int currentDepth = 1;

    while (currentDepth <= depth) {
        transposition_table.clear();
        int my_captures = _whiteStone.getPairCaptured();
        int opp_captures = _blackStone.getPairCaptured();
        int localBestY = bestY, localBestX = bestX, localBestScore = bestScore;
        auto moves = generate_moves(intBoard, iaCell);

        ImmediateMoves imm = detect_immediate_moves(intBoard, iaCell);

        if (!imm.wins.empty()) {
            auto [winY, winX] = *imm.wins.begin();
            auto iaNow = std::chrono::high_resolution_clock::now();
            float iaElapsedMs = std::chrono::duration<float, std::milli>(iaNow - turnStart).count();
            return {winY, winX, 10000000, currentDepth, iaElapsedMs, true};
        }

        int opponent = (iaCell == 1 ? 2 : 1);
        auto opp_wins = detect_immediate_wins(intBoard, opponent);
        if (!opp_wins.empty()) {
            auto [blockY, blockX] = *opp_wins.begin();
            auto iaNow = std::chrono::high_resolution_clock::now();
            float iaElapsedMs = std::chrono::duration<float, std::milli>(iaNow - turnStart).count();
            return {blockY, blockX, 9000000, currentDepth, iaElapsedMs, true};
        }

        if (!imm.ff_threats.empty()) {
            auto [ff_threatsY, ff_threatsX] = *imm.ff_threats.begin();
            auto iaNow = std::chrono::high_resolution_clock::now();
            float iaElapsedMs = std::chrono::duration<float, std::milli>(iaNow - turnStart).count();
            return {ff_threatsY, ff_threatsX, 10000000, currentDepth, iaElapsedMs, true};
        }
        
        if (!imm.free_fours.empty()) {
            auto [ffY, ffX] = *imm.free_fours.begin();
            auto iaNow = std::chrono::high_resolution_clock::now();
            float iaElapsedMs = std::chrono::duration<float, std::milli>(iaNow - turnStart).count();
            return {ffY, ffX, 9000000, currentDepth, iaElapsedMs, true};
        }

        if ((!imm.captures.empty() && imm.ft_threats.empty()) && (imm.captures_threats.empty() || opp_captures <= 4)) {
            std::vector<std::tuple<int, int, int>> cap_scored;
            for (const auto& [capY, capX] : imm.captures) {
                int score = move_priority(intBoard, capY, capX, iaCell, my_captures, opp_captures);
                cap_scored.emplace_back(capY, capX, score);
            }
            std::sort(cap_scored.begin(), cap_scored.end(),
                [](const auto& a, const auto& b) {
                    return std::get<2>(a) > std::get<2>(b);
                });
            auto [capY, capX, capScore] = cap_scored.front();
            auto iaNow = std::chrono::high_resolution_clock::now();
            float iaElapsedMs = std::chrono::duration<float, std::milli>(iaNow - turnStart).count();
            return {capY, capX, capScore, currentDepth, iaElapsedMs, true};
        }

        std::set<std::pair<int, int>> combined_threats = imm.ft_threats;
        combined_threats.insert(imm.captures_threats.begin(), imm.captures_threats.end());
        std::vector<std::pair<int, int>> threat_moves;
        for (auto& move : moves) {
            if (combined_threats.count(move))
                threat_moves.push_back(move);
        }
        if (!threat_moves.empty())
            moves = threat_moves;

        auto free_threes = detect_free_threes(intBoard, iaCell);
        if (!free_threes.empty()) {
            std::vector<std::pair<int, int>> free_three_moves;
            for (auto& move : moves) {
                if (free_threes.count(move))
                    free_three_moves.push_back(move);
            }
            if (!free_three_moves.empty()) {
                moves = free_three_moves;
            }
        }

        std::vector<std::tuple<int, int, int>> move_scores;
        for (auto& move : moves) {
            int score = move_priority(intBoard, move.first, move.second, iaCell, my_captures, opp_captures);
            move_scores.emplace_back(move.first, move.second, score);
        }

        std::sort(move_scores.begin(), move_scores.end(),
            [](const auto& a, const auto& b) {
                return std::get<2>(a) > std::get<2>(b);
            });

        moves.clear();
        for (auto& tup : move_scores)
            moves.emplace_back(std::get<0>(tup), std::get<1>(tup));
        if (moves.size() > 6)
            moves.resize(6);

        for (auto [y, x] : moves) {
            intBoard[y][x] = iaCell;
            bool localTimeout = false;
            int score = minimax(intBoard, currentDepth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(),
                                true, iaCell, iaCell, turnStart, maxTimeMs, localTimeout, my_captures, opp_captures);
            intBoard[y][x] = 0;
            if (localTimeout) {
                timeout = true;
                break;
            }
            if (score > localBestScore || localBestY == -1) {
                localBestScore = score;
                localBestY = y;
                localBestX = x;
            }
        }
        if (!timeout) {
            bestY = localBestY;
            bestX = localBestX;
            bestScore = localBestScore;
            currentDepth++;
        }
        else
            break;
    }
    auto iaNow = std::chrono::high_resolution_clock::now();
    float iaElapsedMs = std::chrono::duration<float, std::milli>(iaNow - turnStart).count();

    if (bestY != -1 && bestX != -1) {
        board[bestY][bestX] = iaCell;
        return {bestY, bestX, bestScore, currentDepth, iaElapsedMs, true};
    }
    return {-1, -1, 0, currentDepth, iaElapsedMs, false};
}

Agent::Result Agent::playHint(std::vector<std::vector<Cell>>& board, Cell playerCell) {
    extern std::unordered_map<std::string, int> transposition_table;

    auto turnStart = std::chrono::high_resolution_clock::now();
    float maxTimeMs = 1000.0f;
    bool timeout = false;
    int depth = this->depth + 5;

    int gridSize = board.size();
    std::vector<std::vector<int>> intBoard(gridSize, std::vector<int>(gridSize, 0));
    for (int y = 0; y < gridSize; ++y)
        for (int x = 0; x < gridSize; ++x)
            intBoard[y][x] = board[y][x];

    int bestY = -1, bestX = -1, bestScore = std::numeric_limits<int>::min();
    int currentDepth = 1;

    while (currentDepth <= depth) {
        transposition_table.clear();
        int my_captures = (playerCell == CELL_BLACK) ? _blackStone.getPairCaptured() : _whiteStone.getPairCaptured();
        int opp_captures = (playerCell == CELL_BLACK) ? _whiteStone.getPairCaptured() : _blackStone.getPairCaptured();
        int localBestY = bestY, localBestX = bestX, localBestScore = bestScore;
        auto moves = generate_moves(intBoard, playerCell);

        ImmediateMoves imm = detect_immediate_moves(intBoard, playerCell);

        if (!imm.wins.empty()) {
            auto [winY, winX] = *imm.wins.begin();
            auto iaNow = std::chrono::high_resolution_clock::now();
            float iaElapsedMs = std::chrono::duration<float, std::milli>(iaNow - turnStart).count();
            return {winY, winX, 10000000, currentDepth, iaElapsedMs, true};
        }

        int opponent = (playerCell == 1 ? 2 : 1);
        auto opp_wins = detect_immediate_wins(intBoard, opponent);
        if (!opp_wins.empty()) {
            auto [blockY, blockX] = *opp_wins.begin();
            auto iaNow = std::chrono::high_resolution_clock::now();
            float iaElapsedMs = std::chrono::duration<float, std::milli>(iaNow - turnStart).count();
            return {blockY, blockX, 9000000, currentDepth, iaElapsedMs, true};
        }

        if (!imm.ff_threats.empty()) {
            auto [ff_threatsY, ff_threatsX] = *imm.ff_threats.begin();
            auto iaNow = std::chrono::high_resolution_clock::now();
            float iaElapsedMs = std::chrono::duration<float, std::milli>(iaNow - turnStart).count();
            return {ff_threatsY, ff_threatsX, 10000000, currentDepth, iaElapsedMs, true};
        }
        
        if (!imm.free_fours.empty()) {
            auto [ffY, ffX] = *imm.free_fours.begin();
            auto iaNow = std::chrono::high_resolution_clock::now();
            float iaElapsedMs = std::chrono::duration<float, std::milli>(iaNow - turnStart).count();
            return {ffY, ffX, 9000000, currentDepth, iaElapsedMs, true};
        }

        if ((!imm.captures.empty() && imm.ft_threats.empty()) && (imm.captures_threats.empty() || opp_captures <= 4)) {
            std::vector<std::tuple<int, int, int>> cap_scored;
            for (const auto& [capY, capX] : imm.captures) {
                int score = move_priority(intBoard, capY, capX, playerCell, my_captures, opp_captures);
                cap_scored.emplace_back(capY, capX, score);
            }
            std::sort(cap_scored.begin(), cap_scored.end(),
                [](const auto& a, const auto& b) {
                    return std::get<2>(a) > std::get<2>(b);
                });
            auto [capY, capX, capScore] = cap_scored.front();
            auto iaNow = std::chrono::high_resolution_clock::now();
            float iaElapsedMs = std::chrono::duration<float, std::milli>(iaNow - turnStart).count();
            return {capY, capX, capScore, currentDepth, iaElapsedMs, true};
        }

        std::set<std::pair<int, int>> combined_threats = imm.ft_threats;
        combined_threats.insert(imm.captures_threats.begin(), imm.captures_threats.end());
        std::vector<std::pair<int, int>> threat_moves;
        for (auto& move : moves) {
            if (combined_threats.count(move))
                threat_moves.push_back(move);
        }
        if (!threat_moves.empty())
            moves = threat_moves;

        auto free_threes = detect_free_threes(intBoard, playerCell);
        if (!free_threes.empty()) {
            std::vector<std::pair<int, int>> free_three_moves;
            for (auto& move : moves) {
                if (free_threes.count(move))
                    free_three_moves.push_back(move);
            }
            if (!free_three_moves.empty()) {
                moves = free_three_moves;
            }
        }

        std::vector<std::tuple<int, int, int>> move_scores;
        for (auto& move : moves) {
            int score = move_priority(intBoard, move.first, move.second, playerCell, my_captures, opp_captures);
            move_scores.emplace_back(move.first, move.second, score);
        }

        std::sort(move_scores.begin(), move_scores.end(),
            [](const auto& a, const auto& b) {
                return std::get<2>(a) > std::get<2>(b);
            });

        moves.clear();
        for (auto& tup : move_scores)
            moves.emplace_back(std::get<0>(tup), std::get<1>(tup));
        if (moves.size() > 6)
            moves.resize(6);

        for (auto [y, x] : moves) {
            intBoard[y][x] = playerCell;
            bool localTimeout = false;
            int score = minimax(intBoard, currentDepth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(),
                                true, playerCell, playerCell, turnStart, maxTimeMs, localTimeout, my_captures, opp_captures);
            intBoard[y][x] = 0;
            if (localTimeout) {
                timeout = true;
                break;
            }
            if (score > localBestScore || localBestY == -1) {
                localBestScore = score;
                localBestY = y;
                localBestX = x;
            }
        }
        if (!timeout) {
            bestY = localBestY;
            bestX = localBestX;
            bestScore = localBestScore;
            currentDepth++;
        }
        else
            break;
    }
    auto iaNow = std::chrono::high_resolution_clock::now();
    float iaElapsedMs = std::chrono::duration<float, std::milli>(iaNow - turnStart).count();

    if (bestY != -1 && bestX != -1) {
        return {bestY, bestX, bestScore, currentDepth, iaElapsedMs, true};
    }
    return {-1, -1, 0, currentDepth, iaElapsedMs, false};
}