#ifndef __SEARCH_H
#define __SEARCH_H

#include "board.h"
#include "move.h"
#include <limits.h>
#include <stdlib.h>
#include <time.h>

extern int nodes;
extern int terminate_search;
extern time_t search_time;
extern time_t max_search_time;

int eval();

struct move get_move(time_t available_time);
int alpha_beta(int alpha, int beta, int depth);
int quiesce(int alpha, int beta, int depth);

void check_in();

#endif
