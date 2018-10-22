/*
 * Task:
 * Write a program that creates a child process, and then in the child
 * closes standard output (STDOUT_FILENO). What happens if the child
 * calls printf() to print some output after closing the descriptor?
 *
 * Answer: It looks like nothing happens, i.e. printf just doesn't
 * print anything if stdout has been closed. I would expect it to
 * throw some kind of error, but it doesn't.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    int rc = fork();

    if (rc == -1) {
        perror("fork");
        exit(1);
    } else if (rc == 0) {
        // child
        printf("child\n");
        close(STDOUT_FILENO);
        printf("after closing stdout\n");
    } else {
        // parent
        printf("parent\n");
    }

    return 0;
}
