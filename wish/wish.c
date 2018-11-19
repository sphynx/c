#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "list.h"

static struct node *path;

static void
err(void)
{
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

static void
init_path(void)
{
    add_elem(&path, "/bin");
}

static void
set_path(char **args)
{
    // Overwrite existing path.
    free_list(path);
    path = NULL;
    while (*args)
        add_elem(&path, *args++);
}

static char
*find_in_path(char *cmd)
{
    struct node *curr;
    char *res = NULL;

    for (curr = path; curr != NULL; curr = curr->next) {
        size_t total_len = strlen(curr->val) + strlen(cmd) + 2;
        char *str = calloc(total_len, 1);
        snprintf(str, total_len, "%s/%s", curr->val, cmd);
        if (access(str, X_OK) == 0) {
            res = str;
            break;
        }
    }

    return res;
}

int main(void)
{
    char *line = NULL;
    size_t cap = 0;
    ssize_t line_len;
    char *sep = " \t\n";

    char *cmd;

    init_path();

    printf("wish> ");
    while ((line_len = getline(&line, &cap, stdin)) > 0) {
        struct node *args_list = NULL;
        char *arg_token;

        cmd = strtok(line, sep);
        while ((arg_token = strtok(NULL, sep)) != NULL)
            add_elem(&args_list, arg_token);

        // Prepare cmd+arguments NULL-terminated array.
        size_t args_no = length(args_list);
        char **args = calloc(args_no + 2, sizeof(char *));

        args[0] = cmd;
        struct node *curr = args_list;
        for (size_t i = 1; i <= args_no; i++) {
            args[i] = curr->val;
            curr = curr->next;
        }
        args[args_no + 1] = NULL;

        if (cmd == NULL) {
            // do nothing
        } else if (strcmp(cmd, "exit") == 0) {
            printf("exiting...\n");
            exit(0);
        } else if (strcmp(cmd, "path") == 0) {
            printf("setting new path...\n");
            set_path(&args[1]); // skip path cmd from args[0]
        } else {
            pid_t rc = fork();
            if (rc < 0) {
                err();
            } else if (rc == 0) {
                char* resolved = find_in_path(cmd);
                if (resolved == NULL) {
                    printf("%s is not found in PATH\n", cmd);
                } else {
                    printf("resolved with PATH to: %s\n", resolved);
                    args[0] = resolved;
                    if (execv(args[0], args) == -1) {
                        err();
                    }
                    free(resolved);
                    free(args);
                }
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
