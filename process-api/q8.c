/*
 * Task: Write a program that creates two children, and connects the
 * standard output of one to the standard input of the other, using
 * the pipe() system call.
 */

/*
 * Comments:
 *
 * I used quite a lot of time to implement that. The main source of
 * confusement was that I haven't realized that `int pipe(int fd[2])`
 * system call, actually updates its argument array, creating those
 * new file descriptors and updating its argument array. For some
 * reason I thought that you have to pass an array with existing two
 * file descriptors to that function and that it somehow connects them
 * through a pipe. Because of that I was not sure how I can connect
 * stdout of one child (basically fd=1) to stdin of the other
 * (basically fd=0, the same in both children and parent). Maybe the
 * man page is not clear enough or I was just not used to the fact
 * that passing an array as an argument is the same as passing a
 * pointer and therefore it allows updating the caller's argument in
 * the callee. Anyway, reading several pages about pipes, `dup2` and
 * file descriptors in "Advanced programming in UNIX environment"
 * helped immensely, making everything very clear.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void) {

    printf("parent pid = %d\n", getpid());

    int fds[2];
    if (pipe(fds) == -1) {
        perror("pipe");
        exit(1);
    }

    int read_end  = fds[0];
    int write_end = fds[1];

    int rc1 = fork();

    if (rc1 < 0) {
        perror("fork1");
        exit(1);
    } else if (rc1 == 0) {
        // Child 1.
        printf("child1 pid = %d\n", getpid());

        // Set stdout to be the write_end of the pipe, conceptually
        // connecting them. Everything we write to stdout after that
        // will be sent to the pipe instead.
        if (dup2(write_end, STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(1);
        }

        // Send those string to the pipe:
        printf("hello from child1\n");
        printf("my pid is: %d\n", getpid());
    } else {
        int rc2 = fork();

        if (rc2 < 0) {
            perror("fork2");
            exit(1);
        } else if (rc2 == 0) {
            // Child 2.
            printf("child2 pid = %d\n", getpid());

            // Set stdin to be the read_end of the pipe, conceptually
            // connecting them.
            if (dup2(read_end, STDIN_FILENO) == -1) {
                perror("dup2");
                exit(1);
            }

            // Read now from stdin line by line (effectively from the
            // pipe).
            char* buffer;
            size_t size;

            while (getline(&buffer, &size, stdin) != -1) {
                printf("[pid:%d], child1 says: [%s]\n", getpid(), buffer);
            }

            free(buffer);
        }
    }
}
