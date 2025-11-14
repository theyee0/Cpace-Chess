#ifndef __MOVE_H
#define __MOVE_H

#include "board.h"

extern enum piece board[BOARD_SIZE];
extern enum color turn;

enum move_type {
        IVM,
        QU, TA
};

struct move {
        enum move_type type;
        unsigned int from;
        unsigned int to;
        enum piece captured;
        enum piece moved;
        int score;
};

struct move_list {
        struct move v[256];
        int n;
};

void make_move(struct move m);
void undo_move(struct move m);

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

void append_move(struct move_list *v,
                 enum move_type type,
                 unsigned int from,
                 unsigned int to,
                 enum piece captured,
                 enum piece moved);

#endif
