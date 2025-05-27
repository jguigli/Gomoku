int pattern_score (int count) {
    switch(count) {
        case 2: return 10;
        case 3: return 100;
        case 4: return 1000;
        case 5: return 10000;
        default: return 0;
    }
}

int evaluate_board(const std::vector<std::vector<int>>&board, int player) {
    int score =0;
    const int SIZE = board.size();

    std::vector<std::pair<int, int>> directions = {
        {1, 0},  // Horizontal
        {0, 1},  // Vertical
        {1, 1}, // Diagonal down-right
        {1, -1} // Diagonal down-left
    };

    for (int x = 0; x < SIZE; ++x) {
        for (int y = 0; y < SIZE; ++y) {
            for (auto [dx, dy] : directions) {
                int count = 0;
                for (int step = 0; step < 5; ++step) {
                    int nx = x + step * dx;
                    int ny = y + step * dy;

                    if (nx >= 0 && ny >= 0 && nx < SIZE && ny < SIZE && board[nx][ny] == player) {
                        count++;
                    } else {
                        break;
                    }
                }

                    score += pattern_score(count);
                }
            }
        }
        return score;
    }
