#include "move.h"

void make_move(struct move m) {
        switch (m.type) {
        case QU:
        case TA:
                board[m.from] = EM;
                board[m.to] = m.moved;
                break;
        }
}

void undo_move(struct move m) {
        switch (m.type) {
        case QU:
        case TA:
                board[m.from] = m.moved;
                board[m.to] = m.captured;
        }
}

void gen_moves(struct move_list *v) {
        int i;

        void (*gen_func[7]) (struct move_list*, unsigned int) = {
                gen_king,
                gen_queen,
                gen_rook,
                gen_bishop,
                gen_knight,
                gen_unicorn,
                gen_pawn
        };

        for (i = 0; i < BOARD_SIZE; i++) {
                if (!is_valid(i)) {
                        continue;
                }

                gen_func[board[i] & PIECE_MASK](v, i);
        }
}

void gen_ta_moves(struct move_list *v) {
        int i;

        void (*gen_func[7]) (struct move_list*, unsigned int) = {
                gen_ta_king,
                gen_ta_queen,
                gen_ta_rook,
                gen_ta_bishop,
                gen_ta_knight,
                gen_ta_unicorn,
                gen_ta_pawn
        };

        for (i = 0; i < BOARD_SIZE; i++) {
                if (!is_valid(i)) {
                        continue;
                }

                gen_func[board[i] & PIECE_MASK](v, i);
        }
}

static void gen_sliding(struct move_list *v, unsigned int square, enum direction *d, int n) {
        int i;
        int target;

        for (i = 0; i < n; i++) {
                target = square + d[i];

                while (is_empty(target)) {
                        append_move(v, QU, square, target, board[target], board[square]);
                        target += d[i];
                }

                if (is_enemy(target)) {
                        append_move(v, TA, square, target, board[target], board[square]);
                }
        }
}

static void gen_ta_sliding(struct move_list *v, unsigned int square, enum direction *d, int n) {
        int i;
        int n_square;

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

static void gen_stepping(struct move_list *v, unsigned int square, enum direction *d, int n) {
        int i;
        int target;
        enum move_type type;

        for (i = 0; i < n; i++) {
                target = square = d[i];

                if (board[target] == IV) {
                        continue;
                }

                type = is_enemy(target) ? TA : QU;

                append_move(v, type, square, target, board[target], board[square]);
        }
}

static void gen_ta_stepping(struct move_list *v, unsigned int square, enum direction *d, int n) {
        int i;
        int target;

        for (i = 0; i < n; i++) {
                target = square = d[i];

                if (is_enemy(target)) {
                        append_move(v, TA, square, target, board[target], board[square]);
                }
        }
}


void gen_king(struct move_list *v, unsigned int square) {
        enum direction d[22] = {
                N, E, S, W, H, L,
                NH, EH, SH, WH,
                NL, EL, SL, WL,
                NEH, SEH, NWH, SWH,
                NEL, SEL, NWL, SWL
        };

        gen_stepping(v, square, d, 21);
}

void gen_ta_king(struct move_list *v, unsigned int square) {
        enum direction d[22] = {
                N, E, S, W, H, L,
                NH, EH, SH, WH,
                NL, EL, SL, WL,
                NEH, SEH, NWH, SWH,
                NEL, SEL, NWL, SWL
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
        enum direction d[6] = {N, E, S, W, H, L};

        gen_sliding(v, square, d, 6);
}

void gen_ta_rook(struct move_list *v, unsigned int square) {
        enum direction d[6] = {N, E, S, W, H, L};

        gen_ta_sliding(v, square, d, 6);
}

void gen_bishop(struct move_list *v, unsigned int square) {
        enum direction d[8] = {
                NH, EH, SH, WH,
                NL, EL, SL, WL
        };

        gen_sliding(v, square, d, 8);
}

void gen_ta_bishop(struct move_list *v, unsigned int square) {
        enum direction d[8] = {
                NH, EH, SH, WH,
                NL, EL, SL, WL
        };

        gen_ta_sliding(v, square, d, 8);
}

void gen_knight(struct move_list *v, unsigned int square) {
        enum direction d[24] = {
                NNE, NNW, NNH, NNL, NHH, NLL,
                EEN, EES, EEH, EEL, EHH, ELL,
                SSE, SSW, SSH, SSL, SHH, SLL,
                WWN, WWS, WWH, WWL, WHH, WLL
        };

        gen_stepping(v, square, d, 24);
}

void gen_ta_knight(struct move_list *v, unsigned int square) {
        enum direction d[24] = {
                NNE, NNW, NNH, NNL, NHH, NLL,
                EEN, EES, EEH, EEL, EHH, ELL,
                SSE, SSW, SSH, SSL, SHH, SLL,
                WWN, WWS, WWH, WWL, WHH, WLL
        };

        gen_ta_stepping(v, square, d, 24);
}

void gen_unicorn(struct move_list *v, unsigned int square) {
        enum direction d[8] = {
                NEH, NEL, SEH, SEL,
                NWH, NWL, SWH, SWL
        };

        gen_sliding(v, square, d, 8);
}

void gen_ta_unicorn(struct move_list *v, unsigned int square) {
        enum direction d[8] = {
                NEH, NEL, SEH, SEL,
                NWH, NWL, SWH, SWL
        };

        gen_sliding(v, square, d, 8);
}


void gen_pawn(struct move_list *v, unsigned int square) {
        enum direction d[4] = {E, W, H, L};
        enum direction forward = (turn == WHITE) ? N : S;
        unsigned int target;
        int i;

        target = square + H;
        if (board[target] == EM) {
                append_move(v, QU, square, target, board[target], board[square]);
        }

        target = square + L;
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
        enum direction d[4] = {E, W, H, L};
        enum direction forward = (turn == WHITE) ? N : S;
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
                 enum move_type type,
                 unsigned int from,
                 unsigned int to,
                 enum piece captured,
                 enum piece moved) {
        struct move n;

        n.type = type;
        n.from = from;
        n.to = to;
        n.captured = captured;
        n.moved = moved;

        v->v[v->n++] = n;
}

