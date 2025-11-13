#include "search.h"

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
        unsigned int square;
        unsigned int piece;

        int white_king = 0;
        int black_king = 0;

        for (i = 0; i < BOARD_NUM; i++) {
                for (j = 7; j < 42; j += 7) {
                        for (k = 1; k <= 5; k++) {
                                square = BOARD_MARGIN + i * 49 + j + k;
                                piece = board[square] & PIECE_MASK;

                                if (piece == 0) {
                                        white_king |= (board[square] == WK);
                                        black_king |= (board[square] == BK);
                                        continue;
                                }

                                score += (piece_color(board[square]) == BLACK) ? -piece_value[piece] : piece_value[piece];
                        }
                }
        }

        if (!white_king) {
                score = -INT_MAX;
        }

        if (!black_king) {
                score = INT_MAX;
        }

        return turn == WHITE ? score : -score;
}

int eval() {
        return sum_material();
}

struct move get_move(int depth) {
        struct move_list v;
        struct move best_move;
        int best_score = -INT_MAX;
        int score;

        int alpha = -INT_MAX;
        int beta = INT_MAX;

        int i;

        gen_moves(&v);

        best_move = v.v[0];

        for (i = 0; i < v.n; i++) {
                make_move(v.v[i]);

                score = -alpha_beta(-beta, -alpha, depth - 1);

                undo_move(v.v[i]);

                if (score > best_score) {
                        best_score = score;
                        best_move = v.v[i];
                }
        }

        return best_move;
}

int alpha_beta(int alpha, int beta, int depth) {
        struct move_list v;
        int score;
        int best_score = -INT_MAX;
        int i;
                
        if (depth == 0) {
                return quiesce(alpha, beta, 5);
        }

        gen_moves(&v);

        if (v.n == 0) {
                return 0;
        }

        for (i = 0; i < v.n; i++) {
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

int quiesce(int alpha, int beta, int depth) {
        const int delta = 200;
        struct move_list v;
        int best_score;
        int score;
        int i;

        if (depth <= 0) {
                return eval();
        }

        score = eval();

        if (score >= beta) {
                return score;
        }

        if (score + delta <= alpha) {
                return score;
        }

        if (score > alpha) {
                alpha = score;
        }

        gen_ta_moves(&v);

        for (i = 0; i < v.n; i++) {
                make_move(v.v[i]);

                score = -quiesce(-(alpha + 1), alpha, depth - 1);
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
