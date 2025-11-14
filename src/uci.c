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

char *export_move(char *buf, struct move m) {
        int board;
        int line;
        int square;

        m.from = BOARD_SIZE - 1 - m.from;
        m.to = BOARD_SIZE - 1 - m.to;

        board = m.from / 49 - 2;
        line = (m.from % 49) / 7 - 1;
        square = 6 - m.from % 7;

        buf[0] = board + 'A';
        buf[1] = line + 'a';
        buf[2] = square + '0';

        buf[3] = '-';

        board = m.to / 49 - 2;
        line = (m.to % 49) / 7 - 1;
        square = 6 - m.to % 7;

        buf[4] = board + 'A';
        buf[5] = line + 'a';
        buf[6] = square + '0';

        buf[7] = '\0';

        return buf;
}

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

void nextsym() {
        cur++;
}

void rewindsym(int n) {
        if (cur > n) {
                cur -= n;
        } else {
                cur = 0;
        }
}

char *cursym() {
        return cur < total ? tokens[cur] : NULL;
}

static int match_str(char *s, char **strings, int n) {
        int i;

        for (i = 0; i < n; i++) {
                if (!strcmp(s, strings[i])) {
                        return i;
                }
        }

        return -1;
}

void command(char *input) {
        char *tok;

        char *valid[12] = {
                "uci", "debug", "isready", "setoption",
                "register", "ucinewgame", "position",
                "go", "stop", "ponderhit", "quit", "printboard"
        };

        void (*function[12])() = {
                uci, debug, isready, setoption,
                reg, ucinewgame, position,
                go, stop, ponderhit, quit, printboard
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

        while (cursym() != NULL && (i = match_str(cursym(), valid, 12)) < 0) {
                nextsym();
        }

        if (cursym() == NULL) {
                printf("Unknown command: '%s'. Enter 'help' for more info\n", original);
                return;
        }

        nextsym();
        function[i]();
}

void uci() {
        printf("uciok\n");
}

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

/* Change so engine responds during search */
void isready() {
        printf("readyok\n");
}

void setoption() {
        printf("Not yet implemented: '%s'.\n", original);
}

void reg() {
        printf("Not yet implemented: '%s'.\n", original);
}

void ucinewgame() {
        turn = WHITE;
        reset_board();
}

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

                assert(cursym() && (!strcmp(cursym(), "startpos") || !strcmp(cursym(), "fen")));
                if (!strcmp(cursym(), "startpos")) {
                        turn = WHITE;
                        reset_board();
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

void go() {
        char buf[10];
        struct move m;

        printf("WARNING: Not fully implemented. Program assumes 30s timer.\n");

        m = get_move(30);

        printf("%s\n", export_move(buf, m));
}

void stop() {
        terminate_search = 1;
}

void ponderhit() {
        printf("Not yet implemented: '%s'.\n", original);
}

void quit() {
        active = 0;
}

void printboard() {
        print_board();
}
