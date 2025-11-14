#include "uci.h"
#include <assert.h>

int active = 1;
int debug_mode = 0;

int parse_fail = 0;

static char original[1024];
static char buf[1024];
static char *tokens[1024];
static int cur;
static int total;

/* Given a move struct, return a string in long algebraic notation */
char *export_move(char *buf, struct move m) {
        int board;
        int rank;
        int file;

        m.from = BOARD_SIZE - 1 - m.from;
        m.to = BOARD_SIZE - 1 - m.to;

        board = m.from / 49 - 2;
        rank = (m.from % 49) / 7;
        file = 5 - m.from % 7;

        buf[0] = board + 'A';
        buf[1] = file + 'a';
        buf[2] = rank + '0';

        buf[3] = '-';

        board = m.to / 49 - 2;
        rank = (m.to % 49) / 7;
        file = 5 - m.to % 7;

        buf[4] = board + 'A';
        buf[5] = file + 'a';
        buf[6] = rank + '0';

        buf[7] = '\0';

        return buf;
}

/* Given a string in long algebraic notation, return a move struct */
struct move parse_move(char *move_code) {
        struct move m;
        int i = 0;

        m.from = BOARD_SIZE - 1;
        m.to = BOARD_SIZE - 1;

        /* TODO: Consider supporting more traditional LAN */

        if (move_code[i] < 'A' || 'E' < move_code[i]) {
                m.type = IVM;
                return m;
        }
        m.from -= 49 * (2 + (move_code[i] - 'A'));

        i++;
        if (move_code[i] < 'a' || 'e' < move_code[i]) {
                m.type = IVM;
                return m;
        }
        m.from -= (7 - 1) - (1 + (move_code[i] - 'a'));

        i++;
        if (move_code[i] < '1' || '9' < move_code[i]) {
                m.type = IVM;
                return m;
        }
        m.from -= 7 * (move_code[i] - '0');

        i++;
        if (move_code[i] != '-') {
                m.type = IVM;
                return m;
        }

        i++;
        if (move_code[i] < 'A' || 'E' < move_code[i]) {
                m.type = IVM;
                return m;
        }
        m.to -= 49 * (2 + (move_code[i] - 'A'));

        i++;
        if (move_code[i] < 'a' || 'e' < move_code[i]) {
                m.type = IVM;
                return m;
        }
        m.to -= 7 - 1 - (1 + (move_code[i] - 'a'));

        i++;
        if (move_code[i] < '1' || '9' < move_code[i]) {
                m.type = IVM;
                return m;
        }
        m.to -= 7 * (move_code[i] - '0');

        i++;

        /* Currently fails to parse promotions */
        if (move_code[i] != '\0' && move_code[i] != '\n') {
                m.type = IVM;
                return m;
        }

        m.moved = board[m.from];
        m.captured = board[m.to];

        if (is_enemy(m.to)) {
                m.type = TA;
        } else {
                m.type = QU;
        }

        return m;
}

/* Step to the next symbol in the sequence */
void nextsym() {
        cur++;
}

/* Step `n` symbols back in the sequence */
void rewindsym(int n) {
        if (cur > n) {
                cur -= n;
        } else {
                cur = 0;
        }
}

/* If valid, return the current symbol */
char *cursym() {
        return cur < total ? tokens[cur] : NULL;
}

/* In an array of strings, find the first index matching a target string */
static int match_str(char *s, char **strings, int n) {
        int i;

        for (i = 0; i < n; i++) {
                if (!strcmp(s, strings[i])) {
                        return i;
                }
        }

        return -1;
}

/* Parse a command given as a string parameter */
void command(char *input) {
        char *tok;

        char *valid[13] = {
                "uci", "debug", "isready", "setoption",
                "register", "ucinewgame", "position",
                "go", "stop", "ponderhit", "quit", "printboard",
                "help"
        };

        void (*function[13])() = {
                uci, debug, isready, setoption,
                reg, ucinewgame, position,
                go, stop, ponderhit, quit, printboard, help
        };

        int i;

        parse_fail = 0;

        cur = 0;
        total = 0;

        strcpy(original, input);
        strcpy(buf, input);

        if (strchr(original, '\n') != NULL) {
                *(strchr(original, '\n')) = '\0';
        }

        tok = strtok(buf, " \n");
        while (tok != NULL) {
                tokens[total++] = tok;
                tok = strtok(NULL, " \n");
        }

        while (cursym() != NULL && (i = match_str(cursym(), valid, 13)) < 0) {
                nextsym();
        }

        if (cursym() == NULL) {
                printf("Unknown command: '%s'. Enter 'help' for more info\n", original);
                return;
        }

        nextsym();
        function[i]();
}

/* Respond to the `uci` command */
void uci() {
        printf("uciok\n");
}

/* Respond to the `debug` command */
void debug() {
        char *valid[2] = {
                "on", "off"
        };

        int i;

        if (cursym() == NULL || (i = match_str(cursym(), valid, 11)) < 0) {
                printf("Unknown command: '%s'. Enter 'help' for more info\n", original);
                return;
        }

        assert(i == 0 || i == 1);

        switch (i) {
        case 0:
                debug_mode = 1;
                break;
        case 1:
                debug_mode = 0;
                break;
        }
}

/* Respond to the `isready` command */
void isready() {
        /* TODO: Change so engine responds during search */

        printf("readyok\n");
}

/* Set engine parameters */
void setoption() {
        printf("Not yet implemented: '%s'.\n", original);
}

/* Register the engine */
void reg() {
        printf("Not yet implemented: '%s'.\n", original);
}

/* Reset the game */
void ucinewgame() {
        turn = WHITE;
        reset_board();
}

/* Check if the input sequence represents a valid FEN string */
static int valid_fen() {
        int cur_line = 0;
        int num_lines = 0;

        char *fen = cursym();

        if (fen == NULL) {
                return 0;
        }

        while (*fen != '\0') {
                if (*fen == '/') {
                        if (cur_line != 5) {
                                return 0;
                        }

                        cur_line = 0;

                        num_lines++;
                        fen++;
                        continue;
                }

                if (!isdigit(*fen) && get_piece(*fen) == IV) {
                        return 0;
                }

                if (isdigit(*fen)) {
                        cur_line += *fen - '0';
                } else {
                        cur_line++;
                }

                fen++;
        }

        if (cur_line != 5) {
                return 0;
        }

        num_lines++;

        nextsym();

        fen = cursym();

        if (fen == NULL || (strcmp(fen, "w") && strcmp(fen, "b"))) {
                rewindsym(1);
                return 0;
        }

        rewindsym(1);

        return num_lines == 25;
}

/* Assuming the sequence contains a valid fen string, load it into the board */
static void parse_pos() {
        char *fen;

        int square;
        int cur_board = 0;
        int cur_line = 0;
        int cur_square = 0;

        enum piece p;

        fen = cursym();

        clear_board();

        while (*fen != '\0') {
                if (*fen == '/') {
                        cur_square = 0;
                        cur_line++;
                        cur_board += cur_line / 5;
                        cur_line %= 5;

                        fen++;
                        continue;
                }

                if (isdigit(*fen)) {
                        cur_square += *fen - '0';
                } else {
                        p = get_piece(*fen);

                        square = BOARD_SIZE - 1;
                        square -= 49 * (cur_board + 2);
                        square -= 7 * (cur_line + 1);
                        square -= (cur_square + 1);

                        board[square] = p;

                        cur_square++;
                }

                fen++;
        }

        nextsym();

        turn = !strcmp(cursym(), "w") ? WHITE : BLACK;

        nextsym();
}

/* Parse a position and moves */
void position() {
        char *tok;
        struct move m;

        int i = 0;

        tok = cursym();

        if (!parse_fail && tok && !strcmp(tok, "fen")) {
                nextsym();
                i++;

                if (!valid_fen()) {
                        parse_fail = 1;
                } else {
                        nextsym();
                        nextsym();

                        i += 2;
                }
        } else if (!parse_fail && tok && !strcmp(tok, "startpos")) {
                nextsym();
                i++;
        } else if (!parse_fail && tok && !strcmp(tok, "current")) {
                nextsym();
                i++;
        } else {
                parse_fail = 1;
        }

        if (!parse_fail && (tok = cursym()) && !strcmp(tok, "moves")) {
                nextsym();
                i++;

                tok = cursym();
                while (tok != NULL) {
                        m = parse_move(tok);

                        if (m.type == IVM) {
                                parse_fail = 1;
                                break;
                        }

                        nextsym();
                        i++;
                        tok = cursym();
                }
        } else {
                parse_fail = 1;
        }

        if (parse_fail) {
                printf("Error reading command: '%s'.\n", original);
        } else {
                rewindsym(i);

                assert(cursym() && (!strcmp(cursym(), "startpos") || !strcmp(cursym(), "fen") || !strcmp(cursym(), "current")));
                if (!strcmp(cursym(), "startpos")) {
                        turn = WHITE;
                        reset_board();
                        nextsym();
                } else if (!strcmp(cursym(), "current")) {
                        nextsym();
                } else {
                        nextsym();
                        parse_pos();
                }

                assert(cursym() && !strcmp(cursym(), "moves"));
                nextsym();

                while (cursym() != NULL) {
                        m = parse_move(cursym());

                        make_move(m);

                        nextsym();
                }
        }
}

/* Begin search unconditionally */
void go() {
        char buf[10];
        struct move m;

        printf("WARNING: Not fully implemented. Program assumes 30s timer.\n");

        m = get_move(30);

        printf("%s\n", export_move(buf, m));
}

/* Terminate a search */
void stop() {
        terminate_search = 1;
}

/* Tell the engine to consider potential responses to a position without having played them */
void ponderhit() {
        printf("Not yet implemented: '%s'.\n", original);
}

/* Quit the program */
void quit() {
        active = 0;
}

/* Print the current board state */
void printboard() {
        print_board();
}

/* Print brief help */
void help() {
        char *helpstrings[4] = {
                "uci - Check if engine is in UCI mode",
                "ucinewgame - Reset the board state and set it to be white's turn",
                "position [current | startpos | fen <fenstring>] moves [<move>...] - Set a position and play some list of moves",
                "go - Search the current position and determine the best move",
                "quit - Quit the program"
        };

        int i;

        for (i = 0; i < 4; i++) {
                printf("%s\n", helpstrings[i]);
        }
}
