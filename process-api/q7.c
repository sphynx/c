/*
 * Task:
 * Write a program that creates a child process, and then in the child
 * closes standard output (STDOUT_FILENO). What happens if the child
 * calls printf() to print some output after closing the descriptor?
 *
 * Answer: It looks like nothing happens, i.e. printf just doesn't
 * print anything if stdout has been closed. I would expect it to
 * throw some kind of error, but it doesn't.
 *
 * Follow-up: now I've just re-read my answer above (after reading
 * about error handling in C in King's book) and realise that I was
 * dumb. Of course, there will be an error! (I presume it should be
 * EBADF -- not a valid file descriptor open for writing, because we
 * boldly closed `stdout`) -- I just didn't check. So now I am
 * checking and indeed it is EBADF: "Bad file descriptor", bingo!
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
        if (printf("after closing stdout\n")) {
            perror("printf");
            exit(1);
        }
    } else {
        // parent
        printf("parent\n");
    }

    return 0;
}
