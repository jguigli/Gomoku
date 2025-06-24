#include "../includes/agent.hpp"

Agent::Agent(int depth, Cell iaCell) : depth(depth), iaCell(iaCell) {}

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

    std::cout << "board is: "<< std::endl;
    for (const auto& row : intBoard) {
        for (int cell : row) {
            std::cout << cell << " ";
        }
        std::cout << std::endl;
    }

    int bestY = -1, bestX = -1, bestScore = std::numeric_limits<int>::min();
    int currentDepth = 1;

    while (currentDepth <= depth) {
        transposition_table.clear();
        int localBestY = bestY, localBestX = bestX, localBestScore = bestScore;
        auto moves = generate_moves(intBoard, iaCell);
        std::cout << "Generated moves: " << std::endl;
        for (const auto& move : moves) {
            std::cout << "(" << move.first << ", " << move.second << ") ";
        }
        std::cout << std::endl;
        auto wins = detect_immediate_wins(intBoard, iaCell);
        if (!wins.empty()) {
            auto [winY, winX] = *wins.begin();
            board[winY][winX] = iaCell;
            auto iaNow = std::chrono::high_resolution_clock::now();
            float iaElapsedMs = std::chrono::duration<float, std::milli>(iaNow - turnStart).count();
            return {winY, winX, 10000000, iaElapsedMs, true};
        }

        int opponent = (iaCell == 1 ? 2 : 1);
        auto threats = detect_immediate_threats(intBoard, opponent);

        if (!threats.empty()) {
            std::vector<std::pair<int, int>> threat_moves;
            for (auto& move : moves) {
                if (threats.count(move))
                    threat_moves.push_back(move);
            }
            if (!threat_moves.empty())
                moves = threat_moves;
            std:: cout << "Detected threats here: " << std::endl;
            for (const auto& move : threat_moves) {
                std::cout << "(" << move.first << ", " << move.second << ") ";  
                std::cout << std::endl;
            }
        }


        auto free_fours = detect_free_fours(intBoard, iaCell);
        if (!free_fours.empty()) {
            std::vector<std::pair<int, int>> free_four_moves;
            for (auto& move : moves) {
                if (free_fours.count(move))
                    free_four_moves.push_back(move);
            }
            if (!free_four_moves.empty()) {
                moves = free_four_moves;
            }
        }

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

        std::cout << "Current moves: " << std::endl;
        for (const auto& move : moves) {
            std::cout << "(" << move.first << ", " << move.second << ") ";
        }
        std::cout << std::endl;

        std::vector<std::tuple<int, int, int>> move_scores;
        for (auto& move : moves) {
            int score = move_priority(intBoard, move.first, move.second, iaCell);
            move_scores.emplace_back(move.first, move.second, score);
            std::cout << "Move (" << move.first << ", " << move.second << ") score: " << score << std::endl;

        std::sort(move_scores.begin(), move_scores.end(),
            [](const auto& a, const auto& b) {
                return std::get<2>(a) > std::get<2>(b);
            });

        moves.clear();
        for (auto& tup : move_scores)
            moves.emplace_back(std::get<0>(tup), std::get<1>(tup));
}
        // std::sort(moves.begin(), moves.end(), [&](const std::pair<int, int>& a, const std::pair<int, int>& b) {
        //     return move_priority(intBoard, a.first, a.second, iaCell) > move_priority(intBoard, b.first, b.second, iaCell);
        // });
        if (moves.size() > 5)
            moves.resize(5);

        for (auto [y, x] : moves) {
            intBoard[y][x] = iaCell;
            bool localTimeout = false;
            int score = minimax(intBoard, currentDepth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(),
                                true, iaCell, iaCell, turnStart, maxTimeMs, localTimeout);
            intBoard[y][x] = 0;
            if (localTimeout) {
                std::cout << "Timeout reached at depth " << currentDepth << std::endl;
                timeout = true;
                break;
            }
            if (score > localBestScore || localBestY == -1) {
                std::cout << "New best move found at depth " << currentDepth << ": (" << y << ", " << x << ") with score " << score << std::endl;
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
            std::cout << "timeout: " <<timeout << std::endl;
            std::cout << "Depth: " << currentDepth - 1 << ", Best Move: (" << bestY << ", " << bestX << "), Score: " << bestScore << std::endl;
        }
        else
            break;
        std::cout << "Current depth: " << currentDepth << std::endl;
    }
    auto iaNow = std::chrono::high_resolution_clock::now();
    float iaElapsedMs = std::chrono::duration<float, std::milli>(iaNow - turnStart).count();

    if (bestY != -1 && bestX != -1) {
        board[bestY][bestX] = iaCell;
        return {bestY, bestX, bestScore, iaElapsedMs, true};
    }
    return {-1, -1, 0, iaElapsedMs, false};
}