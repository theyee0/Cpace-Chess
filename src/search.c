#include "search.h"

int nodes = 0;
int terminate_search = 0;

time_t search_time;
time_t max_search_time = 10;

/* Combines a move and its previously determined score */
struct move_pair {
        int score;
        struct move move;
};

/* Comparison function for comparing moves by score */
static int score_cmp(const void *a, const void *b) {
        const struct move_pair *ma = a;
        const struct move_pair *mb = b;

        if (ma->score >= mb->score) {
                return -1;
        } else {
                return 1;
        }
}

/* Compute the sum of all material in the board, and return white points - black points */
static int sum_material() {
        int piece_value[7] = {
                -1,
                900,
                500,
                300,
                600,
                200,
                100
        };

        int i, j, k;
        int score = 0;
        unsigned int piece;

        int white_king = 0;
        int black_king = 0;

        for (i = BOARD_MARGIN; i < BOARD_SIZE - BOARD_MARGIN; i += 49) {
                for (j = 7; j < 42; j += 7) {
                        for (k = 1; k <= 5; k++) {
                                piece = board[i + j + k] & PIECE_MASK;

                                /* Record if a king has been found */
                                if (piece == 0) {
                                        white_king |= (board[i + j + k] == WK);
                                        black_king |= (board[i + j + k] == BK);
                                        continue;
                                }

                                score += (piece_color(board[i + j + k]) == BLACK) ? -piece_value[piece] : piece_value[piece];
                        }
                }
        }

        /* Return +/- infinity if a king has been captured */
        if (!white_king) {
                score = -INT_MAX;
        }

        if (!black_king) {
                score = INT_MAX;
        }

        return score;
}

/* Compute the difference in number of moves */
static int compute_mobility() {
        const int weight = 5;

        struct move_list v;
        enum color current_turn = turn;

        int white_moves;
        int black_moves;

        turn = WHITE;
        gen_moves(&v);
        white_moves = v.n;

        turn = BLACK;
        gen_moves(&v);
        black_moves = v.n;

        turn = current_turn;

        return (white_moves - black_moves) * weight;
}

/* Combine heuristics to evaluate a chess position. Returns score in centipawns */
int eval() {
        int (*eval_components[2])() = {
                sum_material,
                compute_mobility
        };

        int score;
        int total = 0;
        int i;

        for (i = 0; i < 2; i++) {
                score = eval_components[i]();

                if (abs(score) == INT_MAX) {
                        return turn == WHITE ? score : -score;
                }

                total += score;
        }

        return turn == WHITE ? total : -total;
}

/* Find the optimal move in the current chess position */
struct move get_move(time_t available_time) {
        struct move_pair mv[256];
        struct move_list v;

        struct move ret;

        struct move best_move;
        int best_score;
        int score;

        int alpha = -INT_MAX;
        int beta = INT_MAX;

        int depth = 1;

        int i;

        /* Set time at the beginning of search */
        time(&search_time);
        max_search_time = available_time;
        terminate_search = 0;

        /* Generate moves and store them in a new array */
        gen_moves(&v);

        for (i = 0; i < v.n; i++) {
                mv[i].move = v.v[i];
        }

        /* Perform iterative deepening */
        while (!terminate_search) {
                best_score = -INT_MAX;
                alpha = -INT_MAX;
                beta = INT_MAX;

                best_move = mv[0].move;

                /* Iterate over all moves and perform alpha-beta search */
                for (i = 0; i < v.n; i++) {
                        make_move(mv[i].move);

                        score = -alpha_beta(-beta, -alpha, depth - 1);

                        undo_move(mv[i].move);

                        mv[i].score = score;

                        if (score > best_score) {
                                best_score = score;
                                best_move = mv[i].move;
                        }
                }

                if (!terminate_search) {
                        printf("depth %d: %d\n", depth, turn == WHITE ? best_score : -best_score);
                        ret = best_move;
                        /* Reorder moves using previous result */
                        qsort(mv, v.n, sizeof(*mv), score_cmp);
                }

                depth++;
        }

        return ret;
}

/* Perform alpha-beta search to a fixed depth */
int alpha_beta(int alpha, int beta, int depth) {
        struct move_list v;
        int score;
        int best_score = -INT_MAX;
        int i;

        /* Check in every 1024 nodes */
        nodes++;
        if (nodes & (1 << 10)) {
                check_in();
        }

        if (terminate_search) {
                return 0;
        }

        if (depth == 0) {
                return quiesce(alpha, beta, 5);
        }

        gen_moves(&v);

        if (v.n == 0) {
                return 0;
        }

        for (i = 0; i < v.n && !terminate_search; i++) {
                make_move(v.v[i]);

                /* Null window search */
                score = -alpha_beta(-(alpha + 1), -alpha, depth - 1);

                if (alpha < score && score < beta) {
                        score = -alpha_beta(-beta, -alpha, depth - 1);
                }

                undo_move(v.v[i]);

                if (score > best_score) {
                        best_score = score;
                }

                if (score > alpha) {
                        alpha = score;
                }

                if (alpha >= beta) {
                        return alpha;
                }
        }

        return best_score;
}

/* Perform quiescence search (only check captures) */
int quiesce(int alpha, int beta, int depth) {
        const int delta = 200;
        struct move_list v;
        int best_score = -INT_MAX;
        int score = eval();
        int i;

        /* Check in every 1024 nodes */
        nodes++;
        if (nodes & (1 << 10)) {
                check_in();
        }

        if (terminate_search) {
                return 0;
        }

        if (depth <= 0) {
                return score;
        }

        /* Quit if you have gone above beta */
        if (score >= beta) {
                return score;
        }

        /* Perform delta pruning */
        if (score + delta <= alpha) {
                return score;
        }

        if (score > alpha) {
                alpha = score;
        }

        gen_ta_moves(&v);

        /* Test all tactical (capturing) moves */
        for (i = 0; i < v.n; i++) {
                make_move(v.v[i]);

                score = -quiesce(-(alpha + 1), -alpha, depth - 1);
                if (alpha < score && score < beta) {
                        score = -quiesce(-beta, -alpha, depth - 1);
                }

                undo_move(v.v[i]);

                if (score > best_score) {
                        best_score = score;
                }

                if (score > alpha) {
                        alpha = score;
                }

                if (alpha >= beta) {
                        return alpha;
                }
        }

        return best_score;
}

/* Check whether time is up and a search needs to be terminated */
void check_in() {
        time_t current_time;

        time(&current_time);

        if (current_time - search_time > max_search_time) {
                terminate_search = 1;
        }
}
