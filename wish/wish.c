#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "list.h"

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

        struct node *args_list = NULL;
        char *arg_token;

        while ((arg_token = strtok(NULL, sep)) != NULL)
            add_arg(&args_list, arg_token);

        if (cmd == NULL) {
            // do nothing
        } else if (strcmp(cmd, "exit") == 0) {
            printf("exiting...\n");
            exit(0);
        } else {
            pid_t rc = fork();
            if (rc < 0) {
                err();
            } else if (rc == 0) {
                // Prepare arguments array.
                size_t args_no = length(args_list);
                char **args = calloc(args_no + 2, sizeof(char *));

                args[0] = cmd;
                struct node *curr = args_list;
                for (size_t i = 1; i <= args_no; i++) {
                    args[i] = curr->arg;
                    curr = curr->next;
                }
                args[args_no + 1] = NULL;

                if (execv(args[0], args) == -1) {
                    err();
                }

                free(args);
            } else {
                // Parent.
                if (wait(NULL) == -1) {
                    err();
                }
            }
        }

        free_list(args_list);

        printf("wish> ");
    }

    return 0;
}
