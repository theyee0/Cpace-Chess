#ifndef __SEARCH_H
#define __SEARCH_H

#include "board.h"
#include "move.h"
#include <limits.h>
#include <stdlib.h>

int eval();

struct move get_move(int depth);
int alpha_beta(int alpha, int beta, int depth);
int quiesce(int alpha, int beta, int depth);

#endif
