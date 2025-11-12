#ifndef __MOVE_H
#define __MOVE_H

#include "board.h"

extern enum pieces board[BOARD_SIZE];
extern enum turn turn;

enum move_type {
        QU, TA
}

struct move {
        struct move_type type;
        unsigned int from;
        unsigned int to;
        enum pieces captured;
        enum pieces moved;
}

struct move_list {
        struct move v[256];
        int n;
}

void gen_moves(struct move_list *v);
void gen_ta_moves(struct move_list *v);

void gen_king(struct move_list *v, unsigned int square);
void gen_queen(struct move_list *v, unsigned int square);
void gen_rook(struct move_list *v, unsigned int square);
void gen_bishop(struct move_list *v, unsigned int square);
void gen_knight(struct move_list *v, unsigned int square);
void gen_unicorn(struct move_list *v, unsigned int square);
void gen_pawn(struct move_list *v, unsigned int square);

void gen_ta_king(struct move_list *v, unsigned int square);
void gen_ta_queen(struct move_list *v, unsigned int square);
void gen_ta_rook(struct move_list *v, unsigned int square);
void gen_ta_bishop(struct move_list *v, unsigned int square);
void gen_ta_knight(struct move_list *v, unsigned int square);
void gen_ta_unicorn(struct move_list *v, unsigned int square);
void gen_ta_pawn(struct move_list *v, unsigned int square);

#endif
