#ifndef __UCI_H
#define __UCI_H

#include "search.h"
#include "board.h"
#include "move.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int active;
extern int debug_mode;

struct move parse_move(char*);

void nextsym();
void read();

void command(char *input);

void uci();
void debug();
void isready();
void setoption();
void reg();
void ucinewgame();
void position();
void go();
void stop();
void ponderhit();
void quit();

void printboard();

#endif
