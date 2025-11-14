#include "move.h"
#include <assert.h>

/* Make move and change the turn */
void make_move(struct move m) {
        switch (m.type) {
        case QU:
        case TA:
                board[m.from] = EM;
                board[m.to] = m.moved;
                break;
        default:
                break;
        }

        turn = !turn;
}

/* Make move and change the turn */
void undo_move(struct move m) {
        switch (m.type) {
        case QU:
        case TA:
                board[m.from] = m.moved;
                board[m.to] = m.captured;
                break;
        default:
                break;
        }

        turn = !turn;
}

/* Iterate over all squares in the board and generate moves */
void gen_moves(struct move_list *v) {
        int i, j, k;

        void (*gen_func[7]) (struct move_list*, unsigned int) = {
                gen_king,
                gen_queen,
                gen_rook,
                gen_bishop,
                gen_knight,
                gen_unicorn,
                gen_pawn
        };

        v->n = 0;

        for (i = BOARD_MARGIN; i < BOARD_SIZE - BOARD_MARGIN; i += 49) {
                for (j = 7; j < 42; j += 7) {
                        for (k = 1; k <= 5; k++) {
                                if (!is_friendly(i + j + k)) {
                                        continue;
                                }

                                gen_func[board[i + j + k] & PIECE_MASK](v, i + j + k);
                        }
                }
        }
}

/* Iterate over all squares and generate all capture moves */
void gen_ta_moves(struct move_list *v) {
        int i, j, k;

        void (*gen_func[7]) (struct move_list*, unsigned int) = {
                gen_ta_king,
                gen_ta_queen,
                gen_ta_rook,
                gen_ta_bishop,
                gen_ta_knight,
                gen_ta_unicorn,
                gen_ta_pawn
        };

        v->n = 0;


        for (i = BOARD_MARGIN; i < BOARD_SIZE - BOARD_MARGIN; i += 49) {
                for (j = 7; j < 42; j += 7) {
                        for (k = 1; k <= 5; k++) {
                                if (!is_friendly(i + j + k)) {
                                        continue;
                                }

                                gen_func[board[i + j + k] & PIECE_MASK](v, i + j + k);
                        }
                }
        }
}

/* Generate line moves as for rooks, queens, bishops */
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

/* Generate line moves that are captures */
static void gen_ta_sliding(struct move_list *v, unsigned int square, enum direction *d, int n) {
        int i;
        int target;

        for (i = 0; i < n; i++) {
                target = square + d[i];

                while (is_empty(target)) {
                        target += d[i];
                }

                if (is_enemy(target)) {
                        append_move(v, TA, square, target, board[target], board[square]);
                }
        }
}

/* Generate stepping moves like kings and knights */
static void gen_stepping(struct move_list *v, unsigned int square, enum direction *d, int n) {
        int i;
        int target;
        enum move_type type;

        for (i = 0; i < n; i++) {
                target = square + d[i];

                if (is_invalid(i) || is_friendly(i)) {
                        continue;
                }

                type = is_enemy(target) ? TA : QU;

                append_move(v, type, square, target, board[target], board[square]);
        }
}

/* Generate capturing stepping moves */
static void gen_ta_stepping(struct move_list *v, unsigned int square, enum direction *d, int n) {
        int i;
        int target;

        for (i = 0; i < n; i++) {
                target = square + d[i];

                if (is_enemy(target)) {
                        append_move(v, TA, square, target, board[target], board[square]);
                }
        }
}

/* Generate all moves a king would have at a given square */
void gen_king(struct move_list *v, unsigned int square) {
        enum direction d[22] = {
                N, E, S, W, H, L,
                NH, EH, SH, WH,
                NL, EL, SL, WL,
                NEH, SEH, NWH, SWH,
                NEL, SEL, NWL, SWL
        };

        gen_stepping(v, square, d, 22);
}

/* Generate all captures a king would have at a given square */
void gen_ta_king(struct move_list *v, unsigned int square) {
        enum direction d[22] = {
                N, E, S, W, H, L,
                NH, EH, SH, WH,
                NL, EL, SL, WL,
                NEH, SEH, NWH, SWH,
                NEL, SEL, NWL, SWL
        };

        gen_ta_stepping(v, square, d, 22);
}

/* Generate all moves a queen would have at a given square */
void gen_queen(struct move_list *v, unsigned int square) {
        gen_rook(v, square);
        gen_bishop(v, square);
        gen_unicorn(v, square);
}

/* Generate all captures a queen would have at a given square */
void gen_ta_queen(struct move_list *v, unsigned int square) {
        gen_ta_rook(v, square);
        gen_ta_bishop(v, square);
        gen_ta_unicorn(v, square);
}

/* Generate all moves a rook would have at a given square */
void gen_rook(struct move_list *v, unsigned int square) {
        enum direction d[6] = {N, E, S, W, H, L};

        gen_sliding(v, square, d, 6);
}

/* Generate all captures a rook would have at a given square */
void gen_ta_rook(struct move_list *v, unsigned int square) {
        enum direction d[6] = {N, E, S, W, H, L};

        gen_ta_sliding(v, square, d, 6);
}

/* Generate all moves a bishop would have at a given square */
void gen_bishop(struct move_list *v, unsigned int square) {
        enum direction d[8] = {
                NH, EH, SH, WH,
                NL, EL, SL, WL
        };

        gen_sliding(v, square, d, 8);
}

/* Generate all captures a bishop would have at a given square */
void gen_ta_bishop(struct move_list *v, unsigned int square) {
        enum direction d[8] = {
                NH, EH, SH, WH,
                NL, EL, SL, WL
        };

        gen_ta_sliding(v, square, d, 8);
}

/* Generate all moves a knight would have at a given square */
void gen_knight(struct move_list *v, unsigned int square) {
        enum direction d[24] = {
                NNE, NNW, NNH, NNL, NHH, NLL,
                EEN, EES, EEH, EEL, EHH, ELL,
                SSE, SSW, SSH, SSL, SHH, SLL,
                WWN, WWS, WWH, WWL, WHH, WLL
        };

        gen_stepping(v, square, d, 24);
}

/* Generate all captures a knight would have at a given square */
void gen_ta_knight(struct move_list *v, unsigned int square) {
        enum direction d[24] = {
                NNE, NNW, NNH, NNL, NHH, NLL,
                EEN, EES, EEH, EEL, EHH, ELL,
                SSE, SSW, SSH, SSL, SHH, SLL,
                WWN, WWS, WWH, WWL, WHH, WLL
        };

        gen_ta_stepping(v, square, d, 24);
}

/* Generate all moves a unicorn would have at a given square */
void gen_unicorn(struct move_list *v, unsigned int square) {
        enum direction d[8] = {
                NEH, NEL, SEH, SEL,
                NWH, NWL, SWH, SWL
        };

        gen_sliding(v, square, d, 8);
}

/* Generate all captures a unicorn would have at a given square */
void gen_ta_unicorn(struct move_list *v, unsigned int square) {
        enum direction d[8] = {
                NEH, NEL, SEH, SEL,
                NWH, NWL, SWH, SWL
        };

        gen_sliding(v, square, d, 8);
}

/* Generate all moves a pawn would have at a given square */
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

/* Generate all captures a pawn would have at a given square */
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

/* Append a move to a given list */
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

