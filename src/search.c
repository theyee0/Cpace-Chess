#include "search.h"

int eval() {
        return 0;
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

                score = alpha_beta(-beta, -alpha, depth - 1);

                undo_move(v.v[i]);

                if (score > best_score) {
                        score = best_score;
                        best_move = v.v[i];
                }
        }

        return best_move;
}

int alpha_beta(int alpha, int beta, int depth) {
        struct move_list v;
        int score;
        int i;
                
        if (depth == 0) {
                return quiesce(alpha, beta);
        }

        gen_moves(&v);

        if (v.n == 0) {
                return 0;
        }

        for (i = 0; i < v.n; i++) {
                make_move(v.v[i]);
                score = -alpha_beta(-beta, -alpha, depth - 1);
                undo_move(v.v[i]);

                if (score > alpha) {
                        alpha = score;
                }

                if (alpha > beta) {
                        return alpha;
                }
        }

        return alpha;
}

int quiesce(int alpha, int beta) {
        const int delta = 200;

        struct move_list v;
        int score;
        int i;

        score = eval();
        if (score > beta) {
                return score;
        }

        if (score + delta < alpha) {
                return score;
        }

        gen_ta_moves(&v);

        for (i = 0; i < v.n; i++) {
                make_move(v.v[i]);
                score = -quiesce(-beta, -alpha);
                undo_move(v.v[i]);

                if (score > alpha) {
                        alpha = score;
                }

                if (alpha > beta) {
                        return alpha;
                }
        }

        return alpha;
}
