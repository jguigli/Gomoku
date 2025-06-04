#include "../includes/ai.hpp"

AIMoveResult play_ai_turn(std::vector<std::vector<Cell>>& board, int profondeur, Cell iaCell) {
    auto turnStart = std::chrono::high_resolution_clock::now();

    int gridSize = board.size();
    std::vector<std::vector<int>> intBoard(gridSize, std::vector<int>(gridSize, 0));
    for (int y = 0; y < gridSize; ++y)
        for (int x = 0; x < gridSize; ++x)
            intBoard[y][x] = board[y][x];

    int bestY = -1, bestX = -1, bestScore = std::numeric_limits<int>::min();
    for (auto [y, x] : generate_moves(intBoard)) {
        intBoard[y][x] = iaCell;
        int score = minimax(intBoard, profondeur, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), true, iaCell, iaCell);
        intBoard[y][x] = 0;
        if (score > bestScore) {
            bestScore = score;
            bestY = y;
            bestX = x;
        }
    }

    auto iaNow = std::chrono::high_resolution_clock::now();
    float iaElapsedMs = std::chrono::duration<float, std::milli>(iaNow - turnStart).count();

    if (bestY != -1 && bestX != -1) {
        board[bestY][bestX] = iaCell;
        return {bestY, bestX, bestScore, iaElapsedMs, true};
    }
    return {-1, -1, 0, iaElapsedMs, false};
}