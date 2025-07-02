#include "../../includes/agent.hpp"

/**
 * Constructeur de l'Agent IA
 * @param depth: Profondeur maximale de recherche Minimax
 * @param iaCell: Couleur des pierres de l'IA (CELL_BLACK ou CELL_WHITE)
 * @param _blackStone: Référence vers le joueur noir (pour les captures)
 * @param _whiteStone: Référence vers le joueur blanc (pour les captures)
 */
Agent::Agent(int depth, Cell iaCell, Player& _blackStone, Player& _whiteStone) : depth(depth), iaCell(iaCell), _blackStone(_blackStone), _whiteStone(_whiteStone) {}

/**
 * Fonction principale de l'IA - Calcule et joue le meilleur coup
 * Utilise l'approfondissement itératif avec limitation de temps
 */
Agent::Result Agent::play(std::vector<std::vector<Cell>>& board) {
    extern std::unordered_map<std::string, int> transposition_table;

    // Limitation de temps : 500ms maximum par coup
    auto turnStart = std::chrono::high_resolution_clock::now();
    float maxTimeMs = 500.0f;
    bool timeout = false;

    // Conversion du plateau Cell vers int pour les algorithmes
    int gridSize = board.size();
    std::vector<std::vector<int>> intBoard(gridSize, std::vector<int>(gridSize, 0));
    for (int y = 0; y < gridSize; ++y)
        for (int x = 0; x < gridSize; ++x)
            intBoard[y][x] = board[y][x];

    // Variables pour le meilleur coup trouvé
    int bestY = -1, bestX = -1, bestScore = std::numeric_limits<int>::min();
    int currentDepth = 1;

    // **APPROFONDISSEMENT ITÉRATIF** - Augmente la profondeur tant qu'il reste du temps
    while (currentDepth <= depth) {
        transposition_table.clear(); // Nettoyer la table de transposition
        
        // Récupérer le nombre de captures pour chaque joueur
        int my_captures = _whiteStone.getPairCaptured();
        int opp_captures = _blackStone.getPairCaptured();
        
        int localBestY = bestY, localBestX = bestX, localBestScore = bestScore;
        auto moves = generate_moves(intBoard, iaCell);
        
        // **1. DÉTECTION DES COUPS GAGNANTS IMMÉDIATS**
        auto wins = detect_immediate_wins(intBoard, iaCell);
        int opponent = (iaCell == 1 ? 2 : 1);
        auto threats = detect_immediate_threats(intBoard, opponent);
        
        // Si coup gagnant trouvé : le jouer immédiatement !
        if (!wins.empty()) {
            int my_captures = (iaCell == CELL_BLACK) ? _blackStone.getPairCaptured() : _whiteStone.getPairCaptured();
            int opp_captures = (iaCell == CELL_BLACK) ? _whiteStone.getPairCaptured() : _blackStone.getPairCaptured();
            
            // Évaluer tous les coups gagnants et choisir le meilleur
            std::vector<std::tuple<int, int, int>> win_scored;
            for (const auto& [winY, winX] : wins) {
                int score = move_priority(intBoard, winY, winX, iaCell, my_captures, opp_captures);
                win_scored.emplace_back(winY, winX, score);
            }
            // Trier par score décroissant
            std::sort(win_scored.begin(), win_scored.end(),
                [](const auto& a, const auto& b) {
                    return std::get<2>(a) > std::get<2>(b);
                });
            
            auto [winY, winX, winScore] = win_scored.front();
            auto iaNow = std::chrono::high_resolution_clock::now();
            float iaElapsedMs = std::chrono::duration<float, std::milli>(iaNow - turnStart).count();
            return {winY, winX, winScore, currentDepth, iaElapsedMs, true};
        }

        // **2. DÉFENSE CONTRE LES MENACES IMMÉDIATES**
        if (!threats.empty()) {
            // Filtrer les coups pour ne garder que ceux qui bloquent les menaces
            std::vector<std::pair<int, int>> threat_moves;
            for (auto& move : moves) {
                if (threats.count(move))
                    threat_moves.push_back(move);
            }
            if (!threat_moves.empty())
                moves = threat_moves; // Priorité absolue à la défense
        }

        // **3. PRIORISATION DES PATTERNS TACTIQUES FORTS**
        
        // 3a. Quatre libres (quasi-gagnants)
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

        // 3b. Trois libres (créent des menaces multiples)
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

        // **4. ÉVALUATION ET TRI DES COUPS PAR PRIORITÉ**
        std::vector<std::tuple<int, int, int>> move_scores;
        for (auto& move : moves) {
            int score = move_priority(intBoard, move.first, move.second, iaCell, my_captures, opp_captures);
            move_scores.emplace_back(move.first, move.second, score);
        }

        // Trier par score décroissant (meilleurs coups en premier)
        std::sort(move_scores.begin(), move_scores.end(),
            [](const auto& a, const auto& b) {
                return std::get<2>(a) > std::get<2>(b);
            });

        // Limiter à 6 coups maximum pour optimiser les performances
        moves.clear();
        for (auto& tup : move_scores)
            moves.emplace_back(std::get<0>(tup), std::get<1>(tup));
        if (moves.size() > 6)
            moves.resize(6);

        // **5. RECHERCHE MINIMAX POUR CHAQUE COUP CANDIDAT**
        for (auto [y, x] : moves) {
            // Simuler le coup
            intBoard[y][x] = iaCell;
            
            bool localTimeout = false;
            // Lancer la recherche Minimax
            int score = minimax(intBoard, currentDepth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(),
                                true, iaCell, iaCell, turnStart, maxTimeMs, localTimeout, my_captures, opp_captures);
            
            // Annuler la simulation
            intBoard[y][x] = 0;
            
            // Vérifier timeout
            if (localTimeout) {
                timeout = true;
                break;
            }
            
            // Mettre à jour le meilleur coup si nécessaire
            if (score > localBestScore || localBestY == -1) {
                localBestScore = score;
                localBestY = y;
                localBestX = x;
            }
        }
        
        // Si pas de timeout : sauvegarder les résultats et continuer plus profond
        if (!timeout) {
            bestY = localBestY;
            bestX = localBestX;
            bestScore = localBestScore;
            currentDepth++;
        }
        else
            break; // Arrêter si timeout
    }
    
    // Calcul du temps total écoulé
    auto iaNow = std::chrono::high_resolution_clock::now();
    float iaElapsedMs = std::chrono::duration<float, std::milli>(iaNow - turnStart).count();

    // Jouer le meilleur coup trouvé
    if (bestY != -1 && bestX != -1) {
        board[bestY][bestX] = iaCell;
        return {bestY, bestX, bestScore, currentDepth, iaElapsedMs, true};
    }
    return {-1, -1, 0, currentDepth, iaElapsedMs, false};
}

/**
 * Fonction d'aide au joueur - Suggère le meilleur coup
 * Plus de temps alloué (1000ms) et profondeur augmentée pour de meilleurs conseils
 */
Agent::Result Agent::playHint(std::vector<std::vector<Cell>>& board, Cell playerCell) {
    extern std::unordered_map<std::string, int> transposition_table;

    auto turnStart = std::chrono::high_resolution_clock::now();
    float maxTimeMs = 1000.0f; // Plus de temps pour les hints
    bool timeout = false;
    int depth = this->depth + 5; // Profondeur augmentée

    int gridSize = board.size();
    std::vector<std::vector<int>> intBoard(gridSize, std::vector<int>(gridSize, 0));
    for (int y = 0; y < gridSize; ++y)
        for (int x = 0; x < gridSize; ++x)
            intBoard[y][x] = board[y][x];

    int bestY = -1, bestX = -1, bestScore = std::numeric_limits<int>::min();
    int currentDepth = 1;

    // Même logique que play() mais avec plus de temps et de profondeur
    while (currentDepth <= depth) {
        transposition_table.clear();
        int my_captures = (playerCell == CELL_BLACK) ? _blackStone.getPairCaptured() : _whiteStone.getPairCaptured();
        int opp_captures = (playerCell == CELL_BLACK) ? _whiteStone.getPairCaptured() : _blackStone.getPairCaptured();
        int localBestY = bestY, localBestX = bestX, localBestScore = bestScore;
        auto moves = generate_moves(intBoard, playerCell);

        // Même priorisation des coups gagnants
        auto wins = detect_immediate_wins(intBoard, playerCell);
        if (!wins.empty()) {
            int my_captures = (playerCell == CELL_BLACK) ? _blackStone.getPairCaptured() : _whiteStone.getPairCaptured();
            int opp_captures = (playerCell == CELL_BLACK) ? _whiteStone.getPairCaptured() : _blackStone.getPairCaptured();
            std::vector<std::tuple<int, int, int>> win_scored;
            for (const auto& [winY, winX] : wins) {
                int score = move_priority(intBoard, winY, winX, playerCell, my_captures, opp_captures);
                win_scored.emplace_back(winY, winX, score);
            }
            std::sort(win_scored.begin(), win_scored.end(),
                [](const auto& a, const auto& b) {
                    return std::get<2>(a) > std::get<2>(b);
                });
            auto [winY, winX, winScore] = win_scored.front();
            auto iaNow = std::chrono::high_resolution_clock::now();
            float iaElapsedMs = std::chrono::duration<float, std::milli>(iaNow - turnStart).count();
            return {winY, winX, winScore, currentDepth, iaElapsedMs, true};
        }

        int opponent = (playerCell == 1 ? 2 : 1);
        auto threats = detect_immediate_threats(intBoard, opponent);

        if (!threats.empty()) {
            std::vector<std::pair<int, int>> threat_moves;
            for (auto& move : moves) {
                if (threats.count(move))
                    threat_moves.push_back(move);
            }
            if (!threat_moves.empty())
                moves = threat_moves;
        }

        auto free_fours = detect_free_fours(intBoard, playerCell);
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