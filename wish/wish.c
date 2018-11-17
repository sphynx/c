#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

static void
err(void)
{
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

int main(void)
{
    char *line = NULL;
    size_t cap = 0;
    ssize_t line_len;
    char *sep = " \t\n";
    char *cmd;

    printf("wish> ");
    while ((line_len = getline(&line, &cap, stdin)) > 0) {
        cmd = strtok(line, sep);

        if (cmd == NULL) {
            // do nothing
        } else if (strcmp(cmd, "exit") == 0) {
            printf("exiting...\n");
            exit(0);
        } else if (strcmp(cmd, "ls") == 0) {
            pid_t rc = fork();
            if (rc < 0) {
                err();
            } else if (rc == 0) {
                // Child.
                char* args[] = {"/bin/ls", NULL};
                if (execv(args[0], args) == -1) {
                    err();
                }
            } else {
                // Parent.
                if (wait(NULL) == -1) {
                    err();
                }
            }
        } else {
            printf("command not found: %s\n", cmd);
        }

        printf("wish> ");
    }

    return 0;
}
