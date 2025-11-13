#include "uci.h"
#include <stdio.h>

int main() {
        char buf[1024];
        
        reset_board();
        turn = WHITE;

        while (active) {
                fgets(buf, 1024, stdin);
                command(buf);
        }

        return 0;
}
