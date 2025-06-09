#include "../../includes/ai.hpp"
#include <algorithm>
#include <random>
#include <iostream>

AI::AI(Cell aiColor, int maxDepth) : _aiColor(aiColor), _maxDepth(maxDepth), _lastMoveTime(0.0), _timeLimit(5000) {
    _opponentColor = (aiColor == CELL_BLACK) ? CELL_WHITE : CELL_BLACK;
}

AI::~AI() {}

Move AI::getBestMove(Board& board, Player& aiPlayer, Player& opponent) {
    _searchStartTime = std::chrono::high_resolution_clock::now();
    
    // Use iterative deepening with time limit
    Move bestMove = iterativeDeepening(board, aiPlayer, opponent);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - _searchStartTime);
    _lastMoveTime = duration.count() / 1000.0; // Convert to milliseconds
    
    return bestMove;
}

Move AI::iterativeDeepening(Board& board, Player& aiPlayer, Player& opponent) {
    Move bestMove(GRID_SIZE / 2, GRID_SIZE / 2); // Default center move
    
    // First check for immediate threats/wins
    std::vector<Move> threatMoves = generateThreatMoves(board);
    if (!threatMoves.empty()) {
        return threatMoves[0]; // Take the first critical move
    }
    
    // Generate limited set of candidate moves
    std::vector<Move> possibleMoves = generatePossibleMoves(board, true);
    
    if (possibleMoves.empty()) {
        return bestMove; // Return center if no moves
    }
    
    // Quick ordering for better pruning
    orderMoves(possibleMoves, board);
    
    // Iterative deepening from 1 to maxDepth
    for (int depth = 1; depth <= _maxDepth && !isTimeUp(); depth++) {
        int bestScore = INT_MIN;
        Move currentBest = bestMove;
        
        for (const auto& move : possibleMoves) {
            if (isTimeUp()) break;
            
            // Try the move
            board.placePiece(move.x, move.y, Player(_aiColor));
            
            int score = minimax(board, aiPlayer, opponent, depth - 1, INT_MIN, INT_MAX, false, move.x, move.y);
            
            // Undo the move
            board.emptyCell(move.x, move.y);
            
            if (score > bestScore) {
                bestScore = score;
                currentBest = Move(move.x, move.y, score);
            }
        }
        
        // Only update if we completed the depth level
        if (!isTimeUp()) {
            bestMove = currentBest;
        }
    }
    
    return bestMove;
}

int AI::minimax(Board& board, Player& aiPlayer, Player& opponent, int depth, 
                int alpha, int beta, bool isMaximizing, int lastX, int lastY) {
    
    // Time cutoff
    if (isTimeUp()) {
        return evaluateBoard(board, aiPlayer, opponent);
    }
    
    // Check for terminal conditions
    if (lastX != -1 && lastY != -1) {
        Cell lastColor = isMaximizing ? _opponentColor : _aiColor;
        if (checkWinCondition(board, lastX, lastY, lastColor)) {
            return isMaximizing ? -WIN_SCORE : WIN_SCORE;
        }
    }
    
    if (depth == 0) {
        return evaluateBoard(board, aiPlayer, opponent);
    }
    
    std::vector<Move> moves = generatePossibleMoves(board, true);
    if (moves.empty()) {
        return evaluateBoard(board, aiPlayer, opponent);
    }
    
    // Limit moves for performance
    orderMoves(moves, board);
    if (moves.size() > MAX_CANDIDATE_MOVES) {
        moves.resize(MAX_CANDIDATE_MOVES);
    }
    
    if (isMaximizing) {
        int maxEval = INT_MIN;
        for (const auto& move : moves) {
            if (isTimeUp()) break;
            
            board.placePiece(move.x, move.y, Player(_aiColor));
            int eval = minimax(board, aiPlayer, opponent, depth - 1, alpha, beta, false, move.x, move.y);
            board.emptyCell(move.x, move.y);
            
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            
            if (beta <= alpha) break; // Alpha-beta pruning
        }
        return maxEval;
    } else {
        int minEval = INT_MAX;
        for (const auto& move : moves) {
            if (isTimeUp()) break;
            
            board.placePiece(move.x, move.y, Player(_opponentColor));
            int eval = minimax(board, aiPlayer, opponent, depth - 1, alpha, beta, true, move.x, move.y);
            board.emptyCell(move.x, move.y);
            
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            
            if (beta <= alpha) break; // Alpha-beta pruning
        }
        return minEval;
    }
}

int AI::evaluateBoard(Board& board, Player& aiPlayer, Player& opponent) {
    (void)aiPlayer; // Suppress unused parameter warning
    (void)opponent; // Suppress unused parameter warning
    
    int score = 0;
    
    // Simplified evaluation - only check stones and basic patterns
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            Cell cell = board.getCell(x, y);
            if (cell == _aiColor) {
                score += evaluatePosition(board, x, y, _aiColor);
            } else if (cell == _opponentColor) {
                score -= evaluatePosition(board, x, y, _opponentColor);
            }
        }
    }
    
    return score;
}

int AI::evaluatePosition(Board& board, int x, int y, Cell color) {
    int score = 0;
    
    // Evaluate in all four directions
    int directions[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};
    
    for (auto& dir : directions) {
        score += evaluatePattern(board, x, y, dir[0], dir[1], color);
    }
    
    // Small center bonus
    int centerX = GRID_SIZE / 2, centerY = GRID_SIZE / 2;
    int distFromCenter = getManhattanDistance(x, y, centerX, centerY);
    score += (10 - distFromCenter) * CENTER_SCORE / 10;
    
    return score;
}

int AI::evaluatePattern(Board& board, int x, int y, int dx, int dy, Cell color) {
    int consecutive = countConsecutive(board, x, y, dx, dy, color);
    int openEnds = 0;
    
    // Check if both ends are open (simplified)
    int startX = x - dx * (consecutive / 2);
    int startY = y - dy * (consecutive / 2);
    int endX = x + dx * (consecutive / 2);
    int endY = y + dy * (consecutive / 2);
    
    if (isInsideBounds(startX - dx, startY - dy) && board.getCell(startX - dx, startY - dy) == CELL_EMPTY) {
        openEnds++;
    }
    if (isInsideBounds(endX + dx, endY + dy) && board.getCell(endX + dx, endY + dy) == CELL_EMPTY) {
        openEnds++;
    }
    
    // Simplified scoring
    switch (consecutive) {
        case 5: return WIN_SCORE;
        case 4: return FOUR_SCORE;
        case 3: return openEnds == 2 ? THREE_OPEN_SCORE : THREE_HALF_SCORE;
        case 2: return openEnds == 2 ? TWO_OPEN_SCORE : TWO_HALF_SCORE;
        case 1: return ONE_SCORE;
        default: return 0;
    }
}

int AI::countConsecutive(Board& board, int x, int y, int dx, int dy, Cell color) {
    int count = 1;
    
    // Count forward
    int nx = x + dx, ny = y + dy;
    while (isInsideBounds(nx, ny) && board.getCell(nx, ny) == color) {
        count++;
        nx += dx;
        ny += dy;
    }
    
    // Count backward
    nx = x - dx, ny = y - dy;
    while (isInsideBounds(nx, ny) && board.getCell(nx, ny) == color) {
        count++;
        nx -= dx;
        ny -= dy;
    }
    
    return count;
}

bool AI::checkWinCondition(Board& board, int x, int y, Cell color) {
    int directions[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};
    
    for (auto& dir : directions) {
        if (countConsecutive(board, x, y, dir[0], dir[1], color) >= 5) {
            return true;
        }
    }
    return false;
}

bool AI::isTimeUp() const {
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - _searchStartTime);
    return elapsed.count() >= _timeLimit;
}

std::vector<Move> AI::generatePossibleMoves(Board& board, bool limitMoves) {
    (void)limitMoves; // Suppress unused parameter warning
    
    std::vector<Move> moves;
    
    // Find all existing stones first
    std::vector<std::pair<int, int>> stones;
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            if (board.getCell(x, y) != CELL_EMPTY) {
                stones.push_back({x, y});
            }
        }
    }
    
    if (stones.empty()) {
        // Empty board - return center
        moves.push_back(Move(GRID_SIZE / 2, GRID_SIZE / 2));
        return moves;
    }
    
    // Generate moves around stones (1-cell radius only for speed)
    for (const auto& stone : stones) {
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                if (dx == 0 && dy == 0) continue;
                
                int nx = stone.first + dx;
                int ny = stone.second + dy;
                
                if (isValidMove(board, nx, ny)) {
                    moves.push_back(Move(nx, ny));
                }
            }
        }
    }
    
    // Remove duplicates
    std::sort(moves.begin(), moves.end(), [](const Move& a, const Move& b) {
        return a.x < b.x || (a.x == b.x && a.y < b.y);
    });
    moves.erase(std::unique(moves.begin(), moves.end(), [](const Move& a, const Move& b) {
        return a.x == b.x && a.y == b.y;
    }), moves.end());
    
    return moves;
}

std::vector<Move> AI::generateThreatMoves(Board& board) {
    std::vector<Move> threatMoves;
    
    // Check for immediate wins or blocks
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            if (board.getCell(x, y) == CELL_EMPTY) {
                // Check if this move wins for AI
                board.placePiece(x, y, Player(_aiColor));
                if (checkWinCondition(board, x, y, _aiColor)) {
                    board.emptyCell(x, y);
                    threatMoves.push_back(Move(x, y, WIN_SCORE));
                    return threatMoves; // Immediate win
                }
                board.emptyCell(x, y);
                
                // Check if this move blocks opponent win
                board.placePiece(x, y, Player(_opponentColor));
                if (checkWinCondition(board, x, y, _opponentColor)) {
                    board.emptyCell(x, y);
                    threatMoves.push_back(Move(x, y, WIN_SCORE - 1));
                } else {
                    board.emptyCell(x, y);
                }
            }
        }
    }
    
    return threatMoves;
}

void AI::orderMoves(std::vector<Move>& moves, Board& board) {
    // Quick scoring for move ordering
    for (auto& move : moves) {
        move.score = quickEvaluateMove(board, move.x, move.y, _aiColor);
    }
    
    // Sort moves by score (best first)
    std::sort(moves.begin(), moves.end(), [](const Move& a, const Move& b) {
        return a.score > b.score;
    });
}

int AI::quickEvaluateMove(Board& board, int x, int y, Cell color) {
    int score = 0;
    
    // Quick pattern check in all directions
    int directions[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};
    
    for (auto& dir : directions) {
        int consecutive = 1;
        
        // Count in positive direction
        int nx = x + dir[0], ny = y + dir[1];
        while (isInsideBounds(nx, ny) && board.getCell(nx, ny) == color) {
            consecutive++;
            nx += dir[0];
            ny += dir[1];
            if (consecutive >= 3) break; // Early exit
        }
        
        // Count in negative direction
        nx = x - dir[0], ny = y - dir[1];
        while (isInsideBounds(nx, ny) && board.getCell(nx, ny) == color) {
            consecutive++;
            nx -= dir[0];
            ny -= dir[1];
            if (consecutive >= 3) break; // Early exit
        }
        
        // Simple scoring
        if (consecutive >= 3) score += 1000;
        else if (consecutive == 2) score += 100;
        else score += 10;
    }
    
    return score;
}

bool AI::isValidMove(Board& board, int x, int y) {
    return isInsideBounds(x, y) && board.getCell(x, y) == CELL_EMPTY;
}

bool AI::isInsideBounds(int x, int y) const {
    return x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE;
}

Cell AI::getOppositeColor(Cell color) const {
    return (color == CELL_BLACK) ? CELL_WHITE : CELL_BLACK;
}

int AI::getManhattanDistance(int x1, int y1, int x2, int y2) const {
    return abs(x1 - x2) + abs(y1 - y2);
} 