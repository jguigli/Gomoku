#include  <vector>
#include <utility>
#include <algorithm>

// int pattern_score(int count, int openEnds) {
//     switch (count) {
//         case 2:
//             switch (openEnds) {
//                 case 2: return 100;   // 2 alignés ouverts
//                 case 1: return 10;    // 2 alignés fermés
//                 default: return 0;
//             }
//         case 3:
//             switch (openEnds) {
//                 case 2: return 1000;  // 3 alignés ouverts
//                 case 1: return 100;   // 3 alignés fermés
//                 default: return 0;
//             }
//         case 4:
//             switch (openEnds) {
//                 case 2: return 10000; // 4 alignés ouverts
//                 case 1: return 1000;  // 4 alignés fermés
//                 default: return 0;
//             }
//         case 5:
//         default:
//             return 100000; // victoire
//     }
// }

int pattern_score(int count) {
    switch (count) {
        case 2: return 10;
        case 3: return 100;
        case 4: return 1000;
        case 5: return 100000;
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
                    // int openEnds = 0;
                    // int beforeX = x - dx;
                    // int beforeY = y - dy;
                    // int afterX = x + dx * count;
                    // int afterY = y + dy * count;
                    // if (beforeX >= 0 && beforeY >= 0 && beforeX < SIZE && beforeY < SIZE && board[beforeX][beforeY] == 0)
                    //     openEnds++;
                    // if (afterX >= 0 && afterY >= 0 && afterX < SIZE && afterY < SIZE && board[afterX][afterY] == 0)
                    //     openEnds++;
                    // score += pattern_score(count, openEnds);

                    score += pattern_score(count);
                }
            }
        }
        return score;
    }