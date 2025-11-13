#ifndef __BOARD_H
#define __BOARD_H

#include <stdio.h>

#define BOARD_SIZE (49 * 9)
#define BOARD_MARGIN (49 * 2)
#define BOARD_NUM 5
#define BOARD_WIDTH 5
#define BOARD_HEIGHT 5
#define BOARD_AREA 3
#define PIECE_MASK 0xF
#define COLOR_MASK 0x10

enum piece {
        WK = 0x00, BK = 0x10,
        WQ = 0x01, BQ = 0x11,
        WR = 0x02, BR = 0x12,
        WB = 0x03, BB = 0x13,
        WN = 0x04, BN = 0x14,
        WU = 0x05, BU = 0x15,
        WP = 0x06, BP = 0x16,
        EM = 0x20, IV = 0x21
};

enum direction {
        N = 6, E = 1, S = -6, W = -1, H = 36, L = -36,
        NH = N + H, EH = E + H, SH = S + H, WH = W + H,
        NL = N + L, EL = E + L, SL = S + L, WL = W + L,
        NEH = N + E + H, NWH = N + W + H, SEH = S + E + H, SWH = S + W + H,
        NEL = N + E + L, NWL = N + W + L, SEL = S + E + L, SWL = S + W + L,
        NNE = N + N + E, NNW = N + N + W, NNH = N + N + H, NNL = N + N + L,
        EEN = E + E + N, EES = E + E + S, EEH = E + E + H, EEL = E + E + L,
        SSE = S + S + E, SSW = S + S + W, SSH = S + S + H, SSL = S + S + L,
        WWN = W + W + N, WWS = W + W + S, WWH = W + W + H, WWL = W + W + L,
        NHH = N + H + H, NLL = N + L + L,
        EHH = E + H + H, ELL = E + L + L,
        SHH = S + H + H, SLL = S + L + L,
        WHH = W + H + H, WLL = W + L + L
};

enum color {
        WHITE = 0x0,
        BLACK = 0x1
};

extern enum piece board[BOARD_SIZE];
extern enum color turn;

void print_board();
void clear_board();
void reset_board();
void set_board(enum piece*);

enum color piece_color(enum piece piece);

int is_enemy(unsigned int);
int is_empty(unsigned int);
int is_valid(unsigned int);
int is_friendly(unsigned int);

#endif
