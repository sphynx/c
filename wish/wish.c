#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "list.h"

#define PROMPT "wish> "

static struct node *path;

static void
err(char *msg)
{
    // char error_message[30] = "An error has occurred\n";
    // write(STDERR_FILENO, error_message, strlen(error_message));
    fprintf(stderr, "error: %s\n", msg);
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
    while (*args) {
        // `strdup` is essential here, otherwise path entries will be
        // overwritten or freed later.
        add_elem(&path, strdup(*args++));
    }
}

static char
*find_in_path(char *cmd)
{
    struct node *curr;
    for (curr = path; curr != NULL; curr = curr->next) {
        size_t total_len = strlen(curr->val) + strlen(cmd) + 2;
        char *str = calloc(total_len, 1);
        snprintf(str, total_len, "%s/%s", curr->val, cmd);

        if (access(str, X_OK) == 0)
            return str;

        free(str);
    }

    return NULL;
}

int main(void)
{
    char *line = NULL;
    size_t cap = 0;
    ssize_t line_len;
    const char *sep = " \t\n";
    char *cmd;

    init_path();

    printf(PROMPT);
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
        free_list(args_list);

        if (cmd == NULL) {

            // do nothing

        } else if (strcmp(cmd, "exit") == 0) {

            exit(EXIT_SUCCESS);

        } else if (strcmp(cmd, "path") == 0) {

            set_path(&args[1]); // skip path cmd from args[0]

        } else if (strcmp(cmd, "cd") == 0) {

            if (args_no == 1) {
                if (chdir(args[1]) == -1) {
                    perror("chdir");
                    err("chdir failed");
                }
            } else {
                err("cd expects a single argument");
            }
        } else {

            pid_t rc = fork();
            if (rc < 0) {
                perror("fork");
                err("fork failed");
            } else if (rc == 0) {
                char* resolved = find_in_path(cmd);
                if (resolved == NULL) {
                    printf("%s is not found in PATH\n", cmd);
                } else {
                    args[0] = resolved;
                    if (execv(args[0], args) == -1) {
                        perror("execv");
                        err("execv failed");
                    }
                }
                exit(EXIT_FAILURE);
            } else {
                if (wait(NULL) == -1) {
                    perror("wait");
                    err("wait failed");
                }
            }
        }

        free(args);
        printf(PROMPT);
    }

    return EXIT_SUCCESS;
}
