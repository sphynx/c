#include <stdio.h>

int
main(void) {
    long ns, nt, nnl;
    int c;

    ns = nt = nnl = 0;
    while ((c = getchar()) != EOF) {
        if (c == ' ')
            ns++;
        else if (c == '\t')
            nt++;
        else if (c == '\n')
            nnl++;
    }

    printf("spaces=%ld tabs=%ld newlines=%ld\n", ns, nt, nnl);
    return 0;
}
