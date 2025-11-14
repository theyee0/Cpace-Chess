#include "board.h"

enum piece board[BOARD_SIZE];
enum color turn;

/* Return the color of a given piece */
enum color piece_color(enum piece piece) {
        return (piece & 0x10) && 1;
}

/* Return the symbol to be printed on the board, given a piece */
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

/* Print all sub-boards */
void print_board() {
        int i, j, k;
        int square;

        for (i = BOARD_MARGIN; i < BOARD_SIZE - BOARD_MARGIN; i += 49) {
                for (j = 0; j < 49; j += 7) {
                        for (k = 1; k <= 5; k++) {
                                square = i + j + k;
                                printf("%c ", piece_symbol(board[square]));
                        }

                        printf("\n");
                }

                printf("\n");
        }

        printf("\n");
}

/* Fill board with empty squares */
void clear_board() {
        int i, j, k;
        int square;

        for (i = BOARD_MARGIN; i < BOARD_SIZE - BOARD_MARGIN; i += 49) {
                for (j = 7; j < 42; j += 7) {
                        for (k = 1; k <= 5; k++) {
                                square = i + j + k;
                                board[square] = EM;
                        }
                }
        }
}

/* Reset board to the standard Raumschach position */
void reset_board() {
        enum piece standard[BOARD_SIZE] = {
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

/* Initialize the board with a list of values */
void set_board(enum piece *v) {
        int i;

        for (i = 0; i < BOARD_SIZE; i++) {
                board[i] = v[i];
        }
}

/* Check if a given square contains an enemy piece */
int is_enemy(unsigned int square) {
        return board[square] != IV && board[square] != EM && piece_color(board[square]) != turn;
}

/* Given a piece code, return the piece enumeration */
enum piece get_piece(char code) {
        char *white_codes = "KQRBNUP";
        enum piece white_pieces[7] = {
                WK, WQ, WR, WB, WN, WU, WP
        };

        char *black_codes = "kqrbnup";
        enum piece black_pieces[7] = {
                BK, BQ, BR, BB, BN, BU, BP
        };

        int index;

        if (strchr(white_codes, code)) {
                index = strchr(white_codes, code) - white_codes;
                return white_pieces[index];
        } if (strchr(black_codes, code)) {
                index = strchr(black_codes, code) - black_codes;
                return black_pieces[index];
        } else {
                return IV;
        }
}

/* Wrapper to check if a square is empty */
int is_empty(unsigned int square) {
        return board[square] == EM;
}

/* Wrapper to check if a square is valid */
int is_valid(unsigned int square) {
        return board[square] != IV;
}

/* Wrapper to check if a square is invalid */
int is_invalid(unsigned int square) {
        return board[square] == IV;
}

/* Wrapper to check if a square contains a friendly piece */
int is_friendly(unsigned int square) {
        return board[square] != IV && board[square] != EM && piece_color(board[square]) == turn;
}
