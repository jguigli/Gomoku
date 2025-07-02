#include "../../includes/minmax.hpp"

// Table de transposition globale pour mémoriser les positions déjà évaluées
// Optimisation cruciale : évite de recalculer les mêmes positions
std::unordered_map<std::string, int> transposition_table;

/**
 * GÉNÉRATION INTELLIGENTE DES COUPS CANDIDATS
 * Ne génère que les coups pertinents pour réduire l'espace de recherche
 */
std::vector<std::pair<int, int>> generate_moves(const std::vector<std::vector<int>>& board, int player) {
    int SIZE = board.size();
    std::set<std::pair<int, int>> moves;
    std::vector<std::pair<int, int>> stones;

    // Collecter toutes les pierres existantes sur le plateau
    for (int y = 0; y < SIZE; ++y)
        for (int x = 0; x < SIZE; ++x)
            if (board[y][x] != 0)
                stones.emplace_back(y, x);

    // CAS 1: Plateau vide -> jouer au centre
    if (stones.empty()) {
        moves.emplace(SIZE/2, SIZE/2);
    } else {
        // CAS 2: Générer dans une zone limitée autour des pierres existantes
        int min_y = SIZE, max_y = 0, min_x = SIZE, max_x = 0;
        for (auto [sy, sx] : stones) {
            min_y = std::min(min_y, sy);
            max_y = std::max(max_y, sy);
            min_x = std::min(min_x, sx);
            max_x = std::max(max_x, sx);
        }
        
        // Marge adaptative : plus large en début de partie
        int margin = (stones.size() < 6) ? 2 : 1;
        for (int y = std::max(0, min_y - margin); y <= std::min(SIZE - 1, max_y + margin); ++y)
            for (int x = std::max(0, min_x - margin); x <= std::min(SIZE - 1, max_x + margin); ++x)
                if (board[y][x] == 0)
                    moves.emplace(y, x);
    }

    // AJOUT DES COUPS TACTIQUES SPÉCIAUX
    
    // Coups de capture possibles pour ce joueur
    auto capture_moves = detect_capture_moves(board, player);
        for (const auto& pos : capture_moves)
            moves.insert(pos);

    // Trois libres (patterns offensifs très forts)
    auto free_threes = detect_free_threes(board, player);
    for (const auto& pos : free_threes)
        moves.insert(pos);

    // Défense : coups de capture adversaires à bloquer
    int opponent = (player == 1 ? 2 : 1);
    auto capture_threats = detect_capture_moves(board, opponent);
    for (const auto& pos : capture_threats)
        moves.insert(pos);

    // FILTRAGE DES COUPS INTERDITS
    // La règle du double trois libre interdit certains coups
    std::vector<std::pair<int, int>> filtered_moves;
    for (auto& move : moves) {
        // Coup autorisé si :
        // - Il ne crée pas de double trois libre OU
        // - Il crée un double trois libre MAIS permet aussi une capture
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

/**
 * FONCTION DE PRIORITÉ DES COUPS
 * Évalue l'intérêt tactique immédiat d'un coup pour optimiser l'ordre d'évaluation
 */
int move_priority(const std::vector<std::vector<int>>& board, int y, int x, int player, int my_captures, int opp_captures) {
    int SIZE = board.size();
    if (y < 0 || y >= SIZE || x < 0 || x >= SIZE || board[y][x] != 0)
        return 0;

    int score = 0;
    auto tmp = board;
    tmp[y][x] = player;

    // **PRIORITÉ ABSOLUE : Croisement trois libre + quatre**
    // Pattern extrêmement fort qui crée des menaces multiples
    if (is_crossed_free_three_and_four(tmp, y, x, player))
        return 100000000;

    // **COUP GAGNANT IMMÉDIAT**
    int eval_player = evaluate_player(tmp, player);
    if (eval_player >= 1000000)  // 5 pierres alignées
        return 10000000;

    // **PATTERN QUASI-GAGNANT**
    if (eval_player >= 100000)   // 4 pierres alignées
        return score += 100000;

    // **VICTOIRE PAR CAPTURE**
    auto captured_positions = simulate_capture(tmp, y, x, player);
    int captured = captured_positions.size() / 2;
    if (my_captures + captured >= 5)
        return 90000000;

    // Remettre la case vide pour tester la défense
    tmp[y][x] = 0;

    // **DÉFENSE CONTRE COUP GAGNANT ADVERSAIRE**
    tmp[y][x] = 3 - player;  // Simuler coup adversaire
    int eval_opp = evaluate_player(tmp, 3 - player);
    if (eval_opp >= 1000000)     // Bloquer coup gagnant
        return score += 9000000;
    else if (eval_opp >= 10000)  // Bloquer pattern fort
        return score += 900000;

    // **DÉFENSE CONTRE CAPTURE ADVERSAIRE**
    auto opp_captured_positions = simulate_capture(tmp, y, x, 3 - player);
    int opp_captured = opp_captured_positions.size() / 2;
    if (opp_captures + opp_captured >= 5)
        score += 8000000;

    tmp[y][x] = 0;

    // **BONUS DE PROXIMITÉ**
    // Favoriser les coups près des autres pierres (cohésion tactique)
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

/**
 * CONVERSION PLATEAU VERS STRING pour table de transposition
 */
std::string board_to_string(const std:: vector<std::vector<int>> & board){
    std::string result;
    for (const auto& row : board)
        for (int cell : row)
            result += std::to_string(cell);
    return result;
}

/**
 * SIMULATION DE CAPTURE
 * Simule les captures qui se produiraient après avoir joué un coup
 * Modifie le plateau en supprimant les pierres capturées
 */
std::vector<std::pair<int, int>> simulate_capture(std::vector<std::vector<int>>& board, int y, int x, int player) {
    std::vector<std::pair<int, int>> captured_positions;
    int SIZE = board.size();
    int opponent = 3 - player;
    
    // 4 directions principales : horizontal, vertical, diagonale /, diagonale \
    const int directions[4][2] = { {1,0},{0,1},{1,1},{1,-1} };
    
    for (auto& dir : directions) {
        // **CAPTURE DANS DIRECTION POSITIVE**
        // Pattern : Notre_Pierre -> Adversaire -> Adversaire -> Notre_Pierre
        int nx1 = x + dir[0];     // Première pierre adversaire
        int ny1 = y + dir[1];
        int nx2 = x + 2 * dir[0]; // Deuxième pierre adversaire  
        int ny2 = y + 2 * dir[1];
        int nx3 = x + 3 * dir[0]; // Notre pierre qui "ferme" la capture
        int ny3 = y + 3 * dir[1];
        
        if (nx1 >= 0 && nx1 < SIZE && ny1 >= 0 && ny1 < SIZE &&
            nx2 >= 0 && nx2 < SIZE && ny2 >= 0 && ny2 < SIZE &&
            nx3 >= 0 && nx3 < SIZE && ny3 >= 0 && ny3 < SIZE) {
            if (board[ny1][nx1] == opponent &&
                board[ny2][nx2] == opponent &&
                board[ny3][nx3] == player) {
                // Capturer les deux pierres adversaires
                captured_positions.emplace_back(ny1, nx1);
                captured_positions.emplace_back(ny2, nx2);
                board[ny1][nx1] = 0;
                board[ny2][nx2] = 0;
            }
        }
        
        // **CAPTURE DANS DIRECTION NÉGATIVE**
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

/**
 * ALGORITHME MINIMAX AVEC ÉLAGAGE ALPHA-BETA
 * Cœur de l'intelligence artificielle - Explore l'arbre des coups possibles
 */
int minimax(std::vector<std::vector<int>>& board, int depth, int alpha, int beta, bool maximizingPlayer, int player, int currentPlayer, std::chrono::high_resolution_clock::time_point startTime, float maxTimeMs, bool& timeout, int my_captures, int opp_captures) {
    
    // **GESTION DU TIMEOUT**
    auto now = std::chrono::high_resolution_clock::now();
    float elapsed = std::chrono::duration<float, std::milli>(now - startTime).count();
    if (elapsed > maxTimeMs) {
        timeout = true;        
        return 0;
    }
    
    // **DÉTECTION DES SITUATIONS DE FIN DE PARTIE**
    
    // Coup gagnant immédiat pour le joueur actuel
    auto wins = detect_immediate_wins(board, currentPlayer);
    if (!wins.empty())
        return maximizingPlayer ? 10000000 : -10000000;

    // Menace gagnante de l'adversaire (doit être bloquée)
    int opponent = (currentPlayer == 1 ? 2 : 1);
    auto threats = detect_immediate_threats(board, opponent);
    if (!threats.empty())
        return maximizingPlayer ? -9000000 : 9000000;

    // Pattern quasi-gagnant : quatre libre
    auto free_fours = detect_free_fours(board, currentPlayer);
    if (!free_fours.empty())
        return maximizingPlayer ? 8000000 : -8000000;

    // Pattern très fort : trois libre
    auto free_threes = detect_free_threes(board, currentPlayer);
    if (!free_threes.empty())
        return maximizingPlayer ? 5000000 : -5000000;

    // **TABLE DE TRANSPOSITION** - Éviter les recalculs
    std::string hash = board_to_string(board) + std::to_string(currentPlayer) + std::to_string(my_captures) + std::to_string(opp_captures);
    auto it = transposition_table.find(hash);
    if (it != transposition_table.end()) {
        return it->second;
    }
    
    // **GÉNÉRATION ET ÉVALUATION DES COUPS**
    auto moves = generate_moves(board, player);
    
    // Cas terminal : profondeur 0 ou plus de coups
    if (depth == 0 || moves.empty()) {
        int eval = evaluate_board(board, player, currentPlayer, my_captures, opp_captures);
        transposition_table[hash] = eval;
        return eval;
    }

    // **ORDONNANCEMENT DES COUPS** - Évaluer les meilleurs en premier
    std::vector<std::tuple<int, int, int>> move_scores;
    for (auto& move : moves) {
        int score = move_priority(board, move.first, move.second, currentPlayer, my_captures, opp_captures);
        move_scores.emplace_back(move.first, move.second, score);
    }

    // Trier selon la stratégie (maximizing vs minimizing)
    std::sort(move_scores.begin(), move_scores.end(),
        [maximizingPlayer](const auto& a, const auto& b) {
            return maximizingPlayer ? std::get<2>(a) > std::get<2>(b)
                                : std::get<2>(a) < std::get<2>(b);
        });

    // Limiter à 6 coups maximum pour les performances
    moves.clear();
    for (size_t i = 0; i < move_scores.size() && i < 6; ++i)
        moves.emplace_back(std::get<0>(move_scores[i]), std::get<1>(move_scores[i]));

    // **RÉCURSION MINIMAX AVEC ÉLAGAGE ALPHA-BETA**
    
    if (maximizingPlayer) {
        // Joueur maximisant : cherche le meilleur score
        int maxEval = std::numeric_limits<int>::min();
        for (auto [y, x] : moves) {
            // Simuler le coup
            board[y][x] = currentPlayer;
            auto captured_positions = simulate_capture(board, y, x, currentPlayer);
            int captured = captured_positions.size() / 2;
            
            // Mettre à jour les compteurs de capture
            int next_my_captures = my_captures;
            int next_opp_captures = opp_captures;
            if (currentPlayer == player)
                next_my_captures += captured;
            else
                next_opp_captures += captured;
            
            // Appel récursif avec joueur suivant
            int newCurrentPlayer = 3 - currentPlayer;
            int nextDepth = depth - 1;
            int eval = minimax(board, nextDepth, alpha, beta, false, player, newCurrentPlayer,  startTime, maxTimeMs, timeout, next_my_captures, next_opp_captures);
            
            // Annuler la simulation
            board[y][x] = 0;
            for (auto& pos : captured_positions)
                board[pos.first][pos.second] = 3 - currentPlayer;
            
            if (timeout) return maxEval;
            
            // Mise à jour Alpha-Beta
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            
            // **ÉLAGAGE ALPHA-BETA**
            if (beta <= alpha)
                break; // Coupe : pas besoin d'explorer les autres coups
        }
        transposition_table[hash] = maxEval;
        return maxEval;
    } else {
        // Joueur minimisant : cherche le pire score pour l'adversaire
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
            
            // **ÉLAGAGE ALPHA-BETA**
            if (beta <= alpha)
                break;
        }
        transposition_table[hash] = minEval;
        return minEval;
    }
}