#include "../includes/agent.hpp"

Agent::Agent(int depth, Cell iaCell) : depth(depth), iaCell(iaCell) {}

Agent::Result Agent::play(std::vector<std::vector<Cell>>& board) {
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
        int localBestY = bestY, localBestX = bestX, localBestScore = bestScore;
        auto moves = generate_moves(intBoard);
        for (auto [y, x] : moves) {
            intBoard[y][x] = iaCell;
            bool localTimeout = false;
            int score = minimax(intBoard, currentDepth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(),
                                true, iaCell, iaCell, turnStart, maxTimeMs, localTimeout);
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
            std::cout << "Depth: " << currentDepth - 1 << ", Best Move: (" << bestY << ", " << bestX << "), Score: " << bestScore << std::endl;
        } else
            break;
    }
    // for (auto [y, x] : generate_moves(intBoard)) {
    //     intBoard[y][x] = iaCell;
    //     int score = minimax(intBoard, depth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), true, iaCell, iaCell, turnStart, maxTimeMs, timeout);
    //     intBoard[y][x] = 0;
    //     if (score > bestScore) {
    //         bestScore = score;
    //         bestY = y;
    //         bestX = x;
    //     }
    // }

    auto iaNow = std::chrono::high_resolution_clock::now();
    float iaElapsedMs = std::chrono::duration<float, std::milli>(iaNow - turnStart).count();

    if (bestY != -1 && bestX != -1) {
        board[bestY][bestX] = iaCell;
        return {bestY, bestX, bestScore, iaElapsedMs, true};
    }
    return {-1, -1, 0, iaElapsedMs, false};
}