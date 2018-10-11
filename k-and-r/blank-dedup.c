#include <stdio.h>

int
main(void) {
    int in_space, c;

    in_space = 0;
    while ((c = getchar()) != EOF) {
        if (c == ' ') {
            if (!in_space) {
                in_space = 1;
                putchar(c);
            }
        } else {
            if (in_space) {
                in_space = 0;
            }
            putchar(c);
        }
    }

    return 0;
}
