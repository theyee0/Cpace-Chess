#include "move.h"

void gen_moves(struct move_list *v) {
        int i;

        void (*gen_func[7]) (struct move_list*, int) = {
                gen_king,
                gen_queen,
                gen_rook,
                gen_bishop,
                gen_knight,
                gen_unicorn,
                gen_pawn
        }

        for (i = 0; i < BOARD_SIZE; i++) {
                if (!valid_square(i)) {
                        continue;
                }

                gen_func[board[i] & PIECE_MASK](v, i);
        }
}

void gen_ta_moves(struct move_list *v) {
        int i;

        void (*gen_func[7]) (struct move_list*, int) = {
                gen_ta_king,
                gen_ta_queen,
                gen_ta_rook,
                gen_ta_bishop,
                gen_ta_knight,
                gen_ta_unicorn,
                gen_ta_pawn
        }

        for (i = 0; i < BOARD_SIZE; i++) {
                if (!valid_square(i)) {
                        continue;
                }

                gen_func[board[i] & PIECE_MASK](v, i);
        }
}

static void gen_sliding(struct move_list *v, unsigned int square, enum directions *d, int n) {
        int i;
        int target;
        enum move_type type;

        for (i = 0; i < n; i++) {
                target = square + d[i];

                while (target == EM) {
                        append_move(v, QU, square, target, board[target], board[square]);
                        target += d[i];
                }

                if (is_enemy(target)) {
                        append_move(v, TA, square, target, board[target], board[square]);
                }
        }
}

static void gen_ta_sliding(struct move_list *v, unsigned int square, enum directions *d, int n) {
        int i;
        int n_square;
        enum move_type type;

        for (i = 0; i < n; i++) {
                n_square = square + d[i];

                while (n_square == EM) {
                        n_square += d[i];
                }

                if (is_enemy(n_square)) {
                        append_move(v, TA, square, n_square, board[n_square], board[square]);
                }
        }
}

static void gen_stepping(struct move_list *v, unsigned int square, enum directions *d, int n) {
        int i;
        int target;
        enum move_type type;

        for (i = 0; i < n; i++) {
                target = square = d[i];

                if (board[target] == IV) {
                        continue;
                }

                type = is_enemy(n_sq) ? TA : QU;

                append_move(v, type, square, target, board[target], board[square]);
        }
}

static void gen_ta_stepping(struct move_list *v, unsigned int square, enum directions *d, int n) {
        int i;
        int target;
        enum move_type type;

        for (i = 0; i < n; i++) {
                target = square = d[i];

                if (is_enemy(target))
                        append_move(v, TA, square, target, board[n_sq], board[square]);
                }
        }
}


void gen_king(struct move_list *v, unsigned int square) {
        enum directions d[21] = {
                N, E, S, W, U, D,
                NU, EU, SU, WU,
                ND, ED, SD, WD,
                NEU, SEU, NWU, SWU,
                NED, SED, NWD, SWD
        };

        gen_stepping(v, square, d, 21);
}

void gen_ta_king(struct move_list *v, unsigned int square) {
        enum directions d[21] = {
                N, E, S, W, U, D,
                NU, EU, SU, WU,
                ND, ED, SD, WD,
                NEU, SEU, NWU, SWU,
                NED, SED, NWD, SWD
        };

        gen_ta_stepping(v, square, d, 21);
}

void gen_queen(struct move_list *v, unsigned int square) {
        gen_rook(v, square);
        gen_bishop(v, square);
        gen_unicorn(v, square);
}

void gen_ta_queen(struct move_list *v, unsigned int square) {
        gen_ta_rook(v, square);
        gen_ta_bishop(v, square);
        gen_ta_unicorn(v, square);
}

void gen_rook(struct move_list *v, unsigned int square) {
        enum directions d[6] = {N, E, S, W, U, D};

        gen_sliding(v, square, d, 6);
}

void gen_ta_rook(struct move_list *v, unsigned int square) {
        enum directions d[6] = {N, E, S, W, U, D};

        gen_ta_sliding(v, square, d, 6);
}

void gen_bishop(struct move_list *v, unsigned int square) {
        enum directions d[8] = {
                NU, EU, SU, WU,
                ND, ED, SD, WD
        };

        gen_sliding(v, square, d, 8);
}

void gen_ta_bishop(struct move_list *v, unsigned int square) {
        enum directions d[8] = {
                NU, EU, SU, WU,
                ND, ED, SD, WD
        };

        gen_ta_sliding(v, square, d, 8);
}

void gen_knight(struct move_list *v, unsigned int square) {
        enum directions d[24] = {
                NNE, NNW, NNU, NND, NUU, NDD,
                EEN, EES, EEU, EED, EUU, EDD,
                SSE, SSW, SSU, SSD, SUU, SDD,
                WWN, WWS, WWU, WWD, WUU, WDD
        };

        gen_stepping(v, square, d, 24);
}

void gen_ta_knight(struct move_list *v, unsigned int square) {
        enum directions d[24] = {
                NNE, NNW, NNU, NND, NUU, NDD,
                EEN, EES, EEU, EED, EUU, EDD,
                SSE, SSW, SSU, SSD, SUU, SDD,
                WWN, WWS, WWU, WWD, WUU, WDD
        };

        gen_ta_stepping(v, square, d, 24);
}

void gen_unicorn(struct move_list *v, unsigned int square) {
        enum directions d[8] = {
                NEU, NED, SEU, SED,
                NWU, NWD, SWU, SWD
        };

        gen_sliding(v, square, d, 8);
}

void gen_ta_unicorn(struct move_list *v, unsigned int square) {
        enum directions d[8] = {
                NEU, NED, SEU, SED,
                NWU, NWD, SWU, SWD
        };

        gen_sliding(v, square, d, 8);
}


void gen_pawn(struct move_list *v, unsigned int square) {
        enum directions d[4] = {E, W, U, D};
        enum directions forward = (turn == WHITE) ? N : S;
        unsigned int target;
        int i;

        target = square + U;
        if (board[target] == EM) {
                append_move(v, QU, square, target, board[target], board[square]);
        }

        target = square + D;
        if (board[target] == EM) {
                append_move(v, QU, square, target, board[target], board[square]);
        }

        target = square + forward;
        if (board[target] == EM) {
                append_move(v, QU, square, target, board[target], board[square]);
        }

        for (i = 0; i < 4; i++) {
                target = square + forward + d[i];
                if (is_enemy(target)) {
                        append_move(v, TA, square, target, board[target], board[square]);
                }
        }
}

void gen_ta_pawn(struct move_list *v, unsigned int square) {
        enum directions d[4] = {E, W, U, D};
        enum directions forward = (turn == WHITE) ? N : S;
        unsigned int target;
        int i;

        for (i = 0; i < 4; i++) {
                target = square + forward + d[i];
                if (is_enemy(target)) {
                        append_move(v, TA, square, target, board[target], board[square]);
                }
        }
}

void append_move(struct move_list *v,
                 struct move_type type,
                 unsigned int from,
                 unsigned int to,
                 enum pieces captured,
                 enum pieces moved) {
        struct move n;

        n.type = type;
        n.from = from;
        n.to = to;
        n.captured = captured;
        n.moved = moved;

        v.v[v.n++] = n;
}

