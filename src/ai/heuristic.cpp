#include "../../includes/heuristic.hpp"

/**
 * TABLE DES PATTERNS - CŒUR DE L'ÉVALUATION HEURISTIQUE
 * 
 * Système de reconnaissance de patterns pour évaluer les formations de pierres :
 * - 0 = case vide
 * - 1 = pierre du joueur 1 (noir)  
 * - 2 = pierre du joueur 2 (blanc)
 * 
 * Les scores reflètent l'importance tactique :
 * - 1,000,000+ = Formations gagnantes
 * - 10,000+    = Formations quasi-gagnantes  
 * - 1,000      = Formations très fortes
 * - 100        = Formations moyennes
 * - 10         = Formations de base
 */
static std::unordered_map<std::string, int> pattern_table = {
    // **PATTERNS GAGNANTS** - 5 pierres alignées
    {"11111", 1000000},   // Noir : 5 alignées
    {"22222", 1000000},   // Blanc : 5 alignées

    // **PATTERNS QUASI-GAGNANTS** - 4 pierres alignées avec 1 espace
    // Pour le joueur noir (1)
    {"01111", 10000},     // _NNNN (espace au début)
    {"10111", 10000},     // N_NNN (espace en 2e position)
    {"11011", 10000},     // NN_NN (espace au milieu)
    {"11101", 10000},     // NNN_N (espace en 4e position)
    {"11110", 10000},     // NNNN_ (espace à la fin)

    // Pour le joueur blanc (2)
    {"02222", 10000},     // _BBBB
    {"20222", 10000},     // B_BBB
    {"22022", 10000},     // BB_BB
    {"22202", 10000},     // BBB_B
    {"22220", 10000},     // BBBB_

    // **PATTERNS TRES FORTS** - 3 pierres alignées avec espaces ("trois libres")
    // Ces patterns sont particulièrement dangereux car ils créent des menaces multiples
    {"01110", 1000},      // _NNN_ (trois libre parfait)
    {"01011", 1000},      // _N_NN (trois brisé)
    {"01101", 1000},      // _NN_N
    {"10011", 1000},      // N__NN
    {"11001", 1000},      // NN__N
    {"10101", 1000},      // N_N_N

    // Équivalents pour le blanc
    {"02220", 1000},      // _BBB_
    {"02022", 1000},      // _B_BB
    {"02202", 1000},      // _BB_B
    {"20022", 1000},      // B__BB
    {"22002", 1000},      // BB__B
    {"20202", 1000},      // B_B_B

    // **PATTERNS MOYENS** - 2 pierres alignées
    {"00110", 100},       // __NN_
    {"01010", 100},       // _N_N_
    {"01100", 100},       // _NN__
    {"10010", 100},       // N__N_
    {"11000", 100},       // NN___
    {"01001", 100},       // _N__N

    // Équivalents pour le blanc
    {"00220", 100},       // __BB_
    {"02020", 100},       // _B_B_
    {"02200", 100},       // _BB__
    {"20020", 100},       // B__B_
    {"22000", 100},       // BB___
    {"02002", 100},       // _B__B

    // **PATTERNS DE BASE** - Pierres isolées
    {"00010", 10},        // ___N_
    {"00100", 10},        // __N__
    {"01000", 10},        // _N___
    {"10000", 10},        // N____

    {"00020", 10},        // ___B_
    {"00200", 10},        // __B__
    {"02000", 10},        // _B___
    {"20000", 10},        // B____

    // **PATTERNS MIXTES** - Formations bloquées par l'adversaire
    // Valeur réduite car l'extension est limitée
    {"21111", 500},       // B NNNN (4 noirs bloqués par blanc)
    {"12111", 500},       // N_BBB
    {"11211", 500},       // NN_BB
    {"11121", 500},       // NNN_B
    {"11112", 500},       // NNNN B

    {"12222", 500},       // N BBBB
    {"21222", 500},       // B_NNN
    {"22122", 500},       // BB_NN
    {"22212", 500},       // BBB_N
    {"22221", 500},       // BBBB N

    // Patterns bloqués de niveau intermédiaire
    {"21110", 50},        // B NNN_
    {"21011", 50},        // B N_NN
    {"21101", 50},        // B NN_N
    {"20111", 50},        // B_NNN
    {"11012", 50},        // NN_ NB
    {"10112", 50},        // N_ NNB
    {"10121", 50},        // N_ N_NB
    {"11210", 50},        // NNB N_

    {"12220", 50},        // N BBB_
    {"12022", 50},        // N B_BB
    {"12202", 50},        // N BB_B
    {"10222", 50},        // N_BBB
    {"22021", 50},        // BB_ BN
    {"20221", 50},        // B_ BBN
    {"20212", 50},        // B_ B_BN
    {"22120", 50},        // BBN B_

    // Patterns faibles bloqués
    {"21010", 5},         // B N_N_
    {"20101", 5},         // B_N_N
    {"11020", 5},         // NN_B_
    {"12010", 5},         // NB_N_
    {"10120", 5},         // N_NB_
    {"20110", 5},         // B_NN_

    {"12020", 5},         // N B_B_
    {"20102", 5},         // B_N_B
    {"22010", 5},         // BB_N_
    {"10220", 5},         // N_BB_
    {"20210", 5},         // B_BN_
    {"12002", 5}          // N B__B
};

/**
 * CONVERSION D'UNE LIGNE EN PATTERN
 * Extrait un motif de 5 cellules consécutives pour reconnaissance
 */
std::string line_to_pattern(const std::vector<int>& line, int start) {
    std::string s;
    for (int i = 0; i < 5; ++i)
        s += std::to_string(line[start + i]);
    return s;
}

/**
 * ÉVALUATION D'UN JOUEUR SPÉCIFIQUE
 * Parcourt tout le plateau et accumule les scores des patterns trouvés
 */
int evaluate_player(const std::vector<std::vector<int>>& board, int player) {
    int score = 0;
    const int SIZE = board.size();
    
    // 4 directions à analyser : horizontal, vertical, diagonale /, diagonale \
    std::vector<std::pair<int, int>> directions = {{1,0},{0,1},{1,1},{-1,1}};

    // Parcourir chaque position du plateau
    for (int y = 0; y < SIZE; ++y) {
        for (int x = 0; x < SIZE; ++x) {
            // Ne considérer que les positions occupées par le joueur
            if (board[y][x] != player)
                continue;

            // Pour chaque direction, extraire et évaluer les patterns
            for (auto [dx, dy] : directions) {
                std::vector<int> line;
                
                // Construire une ligne de 5 cellules dans cette direction
                for (int k = 0; k < 5; ++k) {
                    int nx = x + dx * k;
                    int ny = y + dy * k;
                    
                    if (nx >= 0 && nx < SIZE && ny >= 0 && ny < SIZE) {
                        // Normaliser : 1 = notre joueur, 0 = vide, 2 = adversaire
                        line.push_back(board[ny][nx] == player ? 1 : (board[ny][nx] == 0 ? 0 : 2));
                    } else {
                        break; // Sortie de plateau
                    }
                }
                
                // Si ligne complète de 5 cellules, chercher le pattern dans la table
                if (line.size() == 5) {
                    std::string pattern;
                    for (int v : line) pattern += std::to_string(v);
                    
                    auto it = pattern_table.find(pattern);
                    if (it != pattern_table.end())
                        score += it->second;
                }
            }
        }
    }
    return score;
}

/**
 * COMPTAGE DES TROIS LIBRES
 * Pattern critique : _OOO_ (3 pierres avec espaces libres aux extrémités)
 * Très dangereux car crée immédiatement deux menaces de 4
 */
int count_free_threes(const std::vector<std::vector<int>>& board, int player) {
    int count = 0;
    int SIZE = board.size();
    std::vector<std::pair<int, int>> directions = {{1,0},{0,1},{1,1},{-1,1}};
    
    for (int y = 0; y < SIZE; ++y) {
        for (int x = 0; x < SIZE; ++x) {
            for (auto [dx, dy] : directions) {
                std::vector<int> line;
                
                // Construire ligne de 5 avec décalage pour détecter _OOO_
                for (int k = -1; k < 4; ++k) {
                    int nx = x + dx * k;
                    int ny = y + dy * k;
                    if (nx >= 0 && nx < SIZE && ny >= 0 && ny < SIZE)
                        line.push_back(board[ny][nx] == player ? 1 : (board[ny][nx] == 0 ? 0 : 2));
                    else
                        line.push_back(2); // Bord = bloqué
                }
                
                // Vérifier pattern exact _OOO_
                if (line.size() == 5) {
                    if (line[0] == 0 && line[1] == 1 && line[2] == 1 && line[3] == 1 && line[4] == 0)
                        count++;
                }
            }
        }
    }
    return count;
}

/**
 * DÉTECTION DES COUPS GAGNANTS IMMÉDIATS
 * Cherche tous les patterns où une seule case vide permet de faire 5 alignés
 */
std::set<std::pair<int, int>> detect_immediate_wins(const std::vector<std::vector<int>>& board, int player) {
    int SIZE = board.size();
    std::set<std::pair<int, int>> wins;
    std::vector<std::pair<int, int>> directions = {{1,0},{0,1},{1,1},{-1,1}};

    // Patterns gagnants : 4 pierres alignées avec 1 espace
    std::vector<std::vector<int>> patterns = {
        {0, player, player, player, player},  // _OOOO
        {player, 0, player, player, player},  // O_OOO
        {player, player, 0, player, player},  // OO_OO
        {player, player, player, 0, player},  // OOO_O
        {player, player, player, player, 0}   // OOOO_
    };

    for (int y = 0; y < SIZE; ++y) {
        for (int x = 0; x < SIZE; ++x) {
            for (auto [dx, dy] : directions) {
                for (const auto& pat : patterns) {
                    int len = pat.size();
                    int empty_y = -1, empty_x = -1;
                    bool match = true;
                    
                    // Vérifier si le pattern correspond
                    for (int k = 0; k < len; ++k) {
                        int nx = x + dx * k;
                        int ny = y + dy * k;
                        
                        if (nx < 0 || nx >= SIZE || ny < 0 || ny >= SIZE) {
                            match = false;
                            break;
                        }
                        
                        int cell = board[ny][nx];
                        if (pat[k] == 0) {
                            if (cell == 0) {
                                empty_y = ny;
                                empty_x = nx;
                            } else {
                                match = false;
                                break;
                            }
                        } else if (cell != pat[k]) {
                            match = false;
                            break;
                        }
                    }
                    
                    // Pattern trouvé : ajouter la case vide comme coup gagnant
                    if (match && empty_y != -1 && empty_x != -1) {
                        wins.insert({empty_y, empty_x});
                    }
                }
            }
        }
    }
    return wins;
}

std::set<std::pair<int, int>> detect_immediate_threats(const std::vector<std::vector<int>>& board, int opponent) {
    int SIZE = board.size();
    std::set<std::pair<int, int>> threats;
    std::vector<std::pair<int, int>> directions = {{1,0},{0,1},{1,1},{-1,1}};

    std::vector<std::vector<int>> patterns = {
        {0, opponent, 0, opponent, opponent, 0},
        {0, opponent, opponent, 0, opponent, 0},
        {0, opponent, opponent, opponent, 0},
        {opponent, opponent, opponent, opponent, 0},
        {0, opponent, opponent, opponent, opponent},
        {0, opponent, opponent, opponent, opponent, 0},
        {opponent, 0, opponent, opponent, opponent},
        {opponent, opponent, 0, opponent, opponent},
        {opponent, opponent, opponent, 0, opponent}
    };

    for (int y = 0; y < SIZE; ++y) {
        for (int x = 0; x < SIZE; ++x) {
            for (auto [dx, dy] : directions) {
                for (const auto& pat : patterns) {
                    int len = pat.size();
                    std::vector<std::pair<int, int>> empty_spots;
                    bool match = true;
                    for (int k = 0; k < len; ++k) {
                        int nx = x + dx * k;
                        int ny = y + dy * k;
                        if (nx < 0 || nx >= SIZE || ny < 0 || ny >= SIZE) {
                            match = false;
                            break;
                        }
                        int cell = board[ny][nx];
                        if (pat[k] == 0) {
                            if (cell == 0)
                                empty_spots.push_back({ny, nx});
                            else {
                                match = false;
                                break;
                            }
                        } else if (pat[k] == opponent) {
                            if (cell != opponent) {
                                match = false;
                                break;
                            }
                        } else {
                            match = false;
                            break;
                        }
                    }
                    if (match && !empty_spots.empty()) {
                        for (auto pos : empty_spots)
                        threats.insert(pos);
                }
            }

            if (checkCapturePair(board, x, y, opponent))
                threats.insert({y, x});
            }
        }
    }
    return threats;
}


bool is_crossed_free_three_and_four(const std::vector<std::vector<int>>& board, int y, int x, int player) {
    if (board[y][x] != 0)
        return false;
    int SIZE = board.size();
    std::vector<std::pair<int, int>> directions = {{1,0},{0,1},{1,1},{-1,1}};
    std::vector<std::vector<int>> free_three_patterns = {
        {0, player, player, player, 0}
    };
    std::vector<std::vector<int>> four_patterns = {
        {player, player, player, player},
        {0, player, player, player, player},
        {player, player, player, player, 0},
        {0, player, player, player, player, 0}
    };

    bool found_free_three = false;
    bool found_four = false;

    for (auto [dx, dy] : directions) {
        for (int offset = -4; offset <= 0; ++offset) {
            for (const auto& pat : free_three_patterns) {
                bool match = true;
                for (int k = 0; k < 5; ++k) {
                    int nx = x + (offset + k) * dx;
                    int ny = y + (offset + k) * dy;
                    if (nx < 0 || nx >= SIZE || ny < 0 || ny >= SIZE) {
                        match = false;
                        break;
                    }
                    int cell = (nx == x && ny == y) ? player : board[ny][nx];
                    if (cell != pat[k]) {
                        match = false;
                        break;
                    }
                }
                if (match)
                    found_free_three = true;
            }
        }
        for (int offset = -5; offset <= 0; ++offset) {
            for (const auto& pat : four_patterns) {
                int len = pat.size();
                bool match = true;
                for (int k = 0; k < len; ++k) {
                    int nx = x + (offset + k) * dx;
                    int ny = y + (offset + k) * dy;
                    if (nx < 0 || nx >= SIZE || ny < 0 || ny >= SIZE) {
                        match = false;
                        break;
                    }
                    int cell = (nx == x && ny == y) ? player : board[ny][nx];
                    if (cell != pat[k]) {
                        match = false;
                        break;
                    }
                }
                if (match)
                    found_four = true;
            }
        }
    }
    return found_free_three && found_four;
}

std::set<std::pair<int, int>> detect_free_threes(const std::vector<std::vector<int>>& board, int player) {
    int SIZE = board.size();
    std::set<std::pair<int, int>> free_threes;
    std::vector<std::pair<int, int>> directions = {{1,0},{0,1},{1,1},{-1,1}};

    std::vector<int> pattern = {0, player, player, player, 0};

    for (int y = 0; y < SIZE; ++y) {
        for (int x = 0; x < SIZE; ++x) {
            for (auto [dx, dy] : directions) {
                bool match = true;
                std::vector<std::pair<int, int>> empty_spots;
                for (int k = 0; k < 5; ++k) {
                    int nx = x + dx * k;
                    int ny = y + dy * k;
                    if (nx < 0 || nx >= SIZE || ny < 0 || ny >= SIZE) {
                        match = false;
                        break;
                    }
                    int cell = board[ny][nx];
                    if (pattern[k] == 0) {
                        if (cell == 0)
                            empty_spots.push_back({ny, nx});
                        else {
                            match = false;
                            break;
                        }
                    } else if (cell != pattern[k]) {
                        match = false;
                        break;
                    }
                }
                if (match && empty_spots.size() == 2) {
                    for (auto pos : empty_spots)
                        free_threes.insert(pos);
                }
            }
        }
    }
    return free_threes;
}

std::set<std::pair<int, int>> detect_free_fours(const std::vector<std::vector<int>>& board, int player) {
    int SIZE = board.size();
    std::set<std::pair<int, int>> free_fours;
    std::vector<std::pair<int, int>> directions = {{1,0},{0,1},{1,1},{-1,1}};

    std::vector<int> pattern = {0, player, player, player, player, 0};

    for (int y = 0; y < SIZE; ++y) {
        for (int x = 0; x < SIZE; ++x) {
            for (auto [dx, dy] : directions) {
                bool match = true;
                std::vector<std::pair<int, int>> empty_spots;
                for (int k = 0; k < 6; ++k) {
                    int nx = x + dx * k;
                    int ny = y + dy * k;
                    if (nx < 0 || nx >= SIZE || ny < 0 || ny >= SIZE) {
                        match = false;
                        break;
                    }
                    int cell = board[ny][nx];
                    if (pattern[k] == 0) {
                        if (cell == 0)
                            empty_spots.push_back({ny, nx});
                        else {
                            match = false;
                            break;
                        }
                    } else if (cell != pattern[k]) {
                        match = false;
                        break;
                    }
                }
                if (match && empty_spots.size() == 2) {
                    for (auto pos : empty_spots)
                        free_fours.insert(pos);
                }
            }
        }
    }
    return free_fours;
}

std::set<std::pair<int, int>> detect_capture_moves(const std::vector<std::vector<int>>& board, int player) {
    int SIZE = board.size();
    std::set<std::pair<int, int>> capture_moves;
    int opponent = (player == 1 ? 2 : 1);
    const int directions[4][2] = {
        {1, 0},
        {0, 1},
        {1, 1},
        {1, -1}
    };

    for (int y = 0; y < SIZE; ++y) {
        for (int x = 0; x < SIZE; ++x) {
            if (board[y][x] != 0)
                continue;
            for (auto& dir : directions) {
                int nx1 = x + dir[0];
                int ny1 = y + dir[1];
                int nx2 = x + 2 * dir[0];
                int ny2 = y + 2 * dir[1];
                int nx3 = x + 3 * dir[0];
                int ny3 = y + 3 * dir[1];
                if (nx1 >= 0 && nx1 < SIZE && ny1 >= 0 && ny1 < SIZE &&
                    nx2 >= 0 && nx2 < SIZE && ny2 >= 0 && ny2 < SIZE &&
                    nx3 >= 0 && nx3 < SIZE && ny3 >= 0 && ny3 < SIZE &&
                    board[ny1][nx1] == opponent &&
                    board[ny2][nx2] == opponent &&
                    board[ny3][nx3] == player) {
                    capture_moves.insert({y, x});
                }
                
                nx1 = x - dir[0];
                ny1 = y - dir[1];
                nx2 = x - 2 * dir[0];
                ny2 = y - 2 * dir[1];
                nx3 = x - 3 * dir[0];
                ny3 = y - 3 * dir[1];
                if (nx1 >= 0 && nx1 < SIZE && ny1 >= 0 && ny1 < SIZE &&
                    nx2 >= 0 && nx2 < SIZE && ny2 >= 0 && ny2 < SIZE &&
                    nx3 >= 0 && nx3 < SIZE && ny3 >= 0 && ny3 < SIZE &&
                    board[ny1][nx1] == opponent &&
                    board[ny2][nx2] == opponent &&
                    board[ny3][nx3] == player) {
                    capture_moves.insert({y, x});
                }
            }
        }
    }
    return capture_moves;
}


/**
 * FONCTION D'ÉVALUATION GLOBALE DU PLATEAU
 * Combine tous les facteurs : patterns, captures, menaces
 */
int evaluate_board(const std::vector<std::vector<int>>& board, int player, int currentPlayer, int my_captures, int opp_captures) {
    int opponent = (player == 1 ? 2 : 1);
    
    // **1. ÉVALUATION DES PATTERNS DE CHAQUE JOUEUR**
    int my_score = evaluate_player(board, player);
    int opp_score = evaluate_player(board, opponent);
    
    // **2. CONDITIONS DE VICTOIRE IMMÉDIATE**
    if (my_score >= 1000000)    // 5 alignés = victoire
        return 10000000;
    if (opp_score >= 1000000)   // Adversaire gagne
        return -10000000;

    // **3. VICTOIRE PAR CAPTURE** (5 paires = 10 pierres)
    if (my_captures >= 5)
        return 10000000;
    if (opp_captures >= 5)
        return -10000000;

    // **4. GESTION DES TROIS LIBRES** (très critiques)
    int opp_free_threes = count_free_threes(board, opponent);
    if (opp_free_threes >= 1)
        return -5000000;  // Menace critique de l'adversaire

    int my_free_threes = count_free_threes(board, player);
    if (my_free_threes >= 1)
        return 5000000;   // Notre menace critique

    // **5. PATTERNS QUASI-GAGNANTS**  
    if (my_score >= 10000)
        return 500000000;  // Notre 4 aligné
    if (opp_score>= 10000)
        return -500000000; // 4 aligné adversaire

    // **6. AJUSTEMENTS SELON LE JOUEUR ACTUEL**
    // Pénaliser si ce n'est pas notre tour et que l'adversaire a des menaces
    if (my_score >= 10000 && currentPlayer != player)
    return -1000000;  // Menace non exploitable immédiatement
    if (opp_score >= 1000 && currentPlayer != player)
    return -500000;   // Pattern fort adversaire

    // **7. SCORE FINAL RELATIF**
    if (currentPlayer == player) {
        return my_score - opp_score;
    }
    else {
        // Pondérer plus fortement l'adversaire quand c'est son tour
        return my_score - (opp_score * 2);
    }
}


bool checkCapturePair(const std::vector<std::vector<int>>& board, int x, int y, int player) {
    if (board[y][x] != 0)
        return false;
    int SIZE = board.size();
    const int directions[4][2] = {
        {1, 0},
        {0, 1},
        {1, 1},
        {1, -1}
    };

    int opponent = (player == 1 ? 2 : 1);

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
                return true;
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
                return true;
            }
        }
    }
    return false;
}


bool spotFreeThreePattern(const std::vector<std::vector<int>>& board, int x, int y, int dx, int dy, int player) {
    int SIZE = board.size();
    std::string pattern;
    const std::vector<std::string> validPatterns = {
        "_OOO_", "_O_OO_", "_OO_O_",
    };

    for (int i = -4; i <= 4; ++i) {
        int nx = x + i * dx;
        int ny = y + i * dy;

        if (nx < 0 || nx >= SIZE || ny < 0 || ny >= SIZE) {
            pattern += "X";
            continue;
        }

        if (nx == x && ny == y) {
            pattern += "O";
            continue;
        }

        int cell = board[ny][nx];
        if (cell == 0)
            pattern += "_";
        else if (cell == player)
            pattern += "O";
        else
            pattern += "X";
    }

    for (const auto& p : validPatterns) {
        if (pattern.find(p) != std::string::npos)
            return true;
    }

    return false;
}

bool checkDoubleFreeThree(const std::vector<std::vector<int>>& board, int x, int y, int player) {
    int SIZE = board.size();
    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE)
        return false;

    int count = 0;
    const int directions[4][2] = {
        {1, 0},
        {0, 1},
        {1, 1},
        {1, -1}
    };

    for (auto& direction : directions) {
        if (spotFreeThreePattern(board, x, y, direction[0], direction[1], player))
            count++;
    }
    return count >= 2;
}