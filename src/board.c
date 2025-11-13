#include "board.h"

enum piece board[BOARD_SIZE];
enum color turn;

enum color piece_color(enum piece piece) {
        return (piece & 0x10) && 1;
}

static char piece_symbol(enum piece piece) {
        char *white_pieces = "KQRBNUP";
        char *black_pieces = "kqrbnup";

        if (piece == EM) {
                return '.';
        } else if (piece == IV) {
                return '-';
        } else if (piece_color(piece) == WHITE) {
                return white_pieces[piece & PIECE_MASK];
        } else {
                return black_pieces[piece & PIECE_MASK];
        }
}

void print_board() {
        int i, j, k;
        int square;

        for (i = 0; i < BOARD_NUM; i++) {
                for (j = 0; j < 49; j += 7) {
                        for (k = 1; k <= 5; k++) {
                                square = BOARD_MARGIN + i * 49 + j + k;
                                printf("%c ", piece_symbol(board[square]));
                        }

                        printf("\n");
                }

                printf("\n");
        }
}

void clear_board() {
        int i, j, k;
        int square;

        for (i = 0; i < BOARD_NUM; i++) {
                for (j = 0; j < 49; j += 7) {
                        for (k = 1; k <= 5; k++) {
                                square = BOARD_MARGIN + i * 49 + j + k;
                                board[square] = EM;
                        }
                }
        }
}

void reset_board() {
        enum piece standard = {
                IV, IV, IV, IV, IV, IV, IV,
                IV, IV, IV, IV, IV, IV, IV,
                IV, IV, IV, IV, IV, IV, IV,
                IV, IV, IV, IV, IV, IV, IV,
                IV, IV, IV, IV, IV, IV, IV,
                IV, IV, IV, IV, IV, IV, IV,
                IV, IV, IV, IV, IV, IV, IV,

                IV, IV, IV, IV, IV, IV, IV,
                IV, IV, IV, IV, IV, IV, IV,
                IV, IV, IV, IV, IV, IV, IV,
                IV, IV, IV, IV, IV, IV, IV,
                IV, IV, IV, IV, IV, IV, IV,
                IV, IV, IV, IV, IV, IV, IV,
                IV, IV, IV, IV, IV, IV, IV,

                IV, IV, IV, IV, IV, IV, IV,
                IV, BR, BN, BK, BN, BR, IV,
                IV, BP, BP, BP, BP, BP, IV,
                IV, EM, EM, EM, EM, EM, IV,
                IV, EM, EM, EM, EM, EM, IV,
                IV, EM, EM, EM, EM, EM, IV,
                IV, IV, IV, IV, IV, IV, IV,

                IV, IV, IV, IV, IV, IV, IV,
                IV, BB, BU, BQ, BB, BU, IV,
                IV, BP, BP, BP, BP, BP, IV,
                IV, EM, EM, EM, EM, EM, IV,
                IV, EM, EM, EM, EM, EM, IV,
                IV, EM, EM, EM, EM, EM, IV,
                IV, IV, IV, IV, IV, IV, IV,

                IV, IV, IV, IV, IV, IV, IV,
                IV, EM, EM, EM, EM, EM, IV,
                IV, EM, EM, EM, EM, EM, IV,
                IV, EM, EM, EM, EM, EM, IV,
                IV, EM, EM, EM, EM, EM, IV,
                IV, EM, EM, EM, EM, EM, IV,
                IV, IV, IV, IV, IV, IV, IV,

                IV, IV, IV, IV, IV, IV, IV,
                IV, EM, EM, EM, EM, EM, IV,
                IV, EM, EM, EM, EM, EM, IV,
                IV, EM, EM, EM, EM, EM, IV,
                IV, WP, WP, WP, WP, WP, IV,
                IV, WB, WU, WQ, WB, WU, IV,
                IV, IV, IV, IV, IV, IV, IV,

                IV, IV, IV, IV, IV, IV, IV,
                IV, EM, EM, EM, EM, EM, IV,
                IV, EM, EM, EM, EM, EM, IV,
                IV, EM, EM, EM, EM, EM, IV,
                IV, WP, WP, WP, WP, WP, IV,
                IV, WR, WN, WK, WN, WR, IV,
                IV, IV, IV, IV, IV, IV, IV,

                IV, IV, IV, IV, IV, IV, IV,
                IV, IV, IV, IV, IV, IV, IV,
                IV, IV, IV, IV, IV, IV, IV,
                IV, IV, IV, IV, IV, IV, IV,
                IV, IV, IV, IV, IV, IV, IV,
                IV, IV, IV, IV, IV, IV, IV,
                IV, IV, IV, IV, IV, IV, IV,

                IV, IV, IV, IV, IV, IV, IV,
                IV, IV, IV, IV, IV, IV, IV,
                IV, IV, IV, IV, IV, IV, IV,
                IV, IV, IV, IV, IV, IV, IV,
                IV, IV, IV, IV, IV, IV, IV,
                IV, IV, IV, IV, IV, IV, IV,
                IV, IV, IV, IV, IV, IV, IV,
        };

        set_board(standard);
}


void set_board(enum piece *v) {
        int i;

        for (i = 0; i < BOARD_SIZE; i++) {
                board[i] = v[i];
        }
}

int is_enemy(unsigned int square) {
        if (board[square] == IV || board[square] == EM) {
                return 0;
        }

        if (!(board[square] & PIECE_MASK) == !turn) {
                return 1;
        }

        return 0;
}

int is_empty(unsigned int square) {
        return board[square] == EM;
}

int is_valid(unsigned int square) {
        return board[square] != IV;
}

int is_friendly(unsigned int square) {
        if (board[square] == IV || board[square] == EM) {
                return 0;
        }

        if (!(board[square] & PIECE_MASK) != !turn) {
                return 1;
        }

        return 0;
}
