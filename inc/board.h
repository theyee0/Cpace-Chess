#ifndef __BOARD_H
#define __BOARD_H

#define BOARD_SIZE (49 * 9)
#define BOARD_MARGIN (49 * 2)
#define BOARD_NUM 5
#define BOARD_WIDTH 5
#define BOARD_HEIGHT 5
#define BOARD_AREA 3
#define PIECE_MASK 0xF

enum pieces {
        WK = 0x00, BK = 0x10,
        WQ = 0x01, BK = 0x11,
        WR = 0x02, BR = 0x12,
        WB = 0x03, BB = 0x13,
        WN = 0x04, BN = 0x14,
        WU = 0x05, BU = 0x15,
        WP = 0x06, BP = 0x16,
        EM = 0x20, IV = 0x21
};

enum directions {
        N, E, S, W, U, D,
        NU, EU, SU, WU,
        ND, ED, SD, WD,
        NEU, NWU, SEU, SWU,
        NED, NWD, SED, SWD,
        NNE, NNW, NNU, NND,
        EEN, EES, EEU, EED,
        SSE, SSW, SSU, SSD,
        WWN, WWS, WWU, WWD,
        NUU, NDD,
        EUU, EDD,
        SUU, SDD,
        WUU, WDD
};

enum color {
        WHITE = 0x0,
        BLACK = 0x1
};

extern enum pieces board[BOARD_SIZE];
extern enum color turn;

void print_board();
void clear_board();
void reset_board();
void set_board(enum pieces*);

enum color piece_color(enum pieces piece);

int is_enemy(unsigned int);
int is_empty(unsigned int);
int is_valid(unsigned int);
int is_friendly(unsigned int);

#endif
