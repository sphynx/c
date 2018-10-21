#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {

    printf("hello world (pid:%d)\n", (int) getpid());

    int x = 10;
    printf("x before fork = %d\n", x);

    int rc = fork();
    if (rc < 0) { // fork failed; exit

        fprintf(stderr, "fork failed\n");
        exit(1);

    } else if (rc == 0) { // child (new process)
        printf("child (pid:%d)\n", (int) getpid());

        x = 11;

        printf("x in child = %d\n", x);
    } else { // parent goes down this path (main)

        printf("parent of %d (pid:%d)\n",
               rc, (int) getpid());
        x = 12;
        printf("x in parent = %d\n", x);
    }

    printf("x in (pid:%d) = %d\n", (int) getpid(), x);
    return 0;
}
