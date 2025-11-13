#include "uci.h"

int active = 1;
int debug_mode = 0;

static char original[1024];
static char buf[1024];
static char *tokens[1024];
static int cur;
static int total;

void nextsym() {
        cur++;
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

        char *valid[11] = {
                "uci", "debug", "isready", "setoption",
                "register", "ucinewgame", "position",
                "go", "stop", "ponderhit", "quit"
        };

        void (*function[11])() = {
                uci, debug, isready, setoption,
                reg, ucinewgame, position,
                go, stop, ponderhit, quit
        };

        int i;

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

        while (cursym() != NULL && (i = match_str(cursym(), valid, 11)) < 0) {
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

void position() {
        printf("Not yet implemented: '%s'.\n", original);
}

void go() {
        printf("Not yet implemented: '%s'.\n", original);
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
