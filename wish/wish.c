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

static void
parse(char *line, char ***args, size_t *args_len)
{
    const char *sep = " \t\n";

    size_t list_len = 0;
    struct node *list = NULL;
    for (char *tok = strtok(line, sep); tok; tok = strtok(NULL, sep)) {
        add_elem(&list, tok);
        list_len++;
    }

    // Prepare arguments NULL-terminated array.
    size_t new_args_len = list_len;
    char **new_args = calloc(new_args_len + 1, sizeof(char *));
    char **curr_args = new_args;

    for (struct node *curr = list; curr; curr = curr->next)
        *curr_args++ = curr->val;

    new_args[new_args_len] = NULL;
    free_list(list);

    *args = new_args;
    *args_len = new_args_len;
}

int main(void)
{
    char *line = NULL;
    size_t cap = 0;
    ssize_t line_len;
    char **args;
    char *cmd;
    size_t args_len;

    init_path();

    printf(PROMPT);
    while ((line_len = getline(&line, &cap, stdin)) > 0) {
        parse(line, &args, &args_len);
        cmd = args[0];

        if (cmd == NULL) {

            // do nothing

        } else if (strcmp(cmd, "exit") == 0) {

            exit(EXIT_SUCCESS);

        } else if (strcmp(cmd, "path") == 0) {

            set_path(&args[1]); // skip path cmd from args[0]

        } else if (strcmp(cmd, "cd") == 0) {

            if (args_len == 2) {
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
