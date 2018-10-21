#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

enum mode { EXECL, EXECLE, EXECLP, EXECV, EXECVP, EXECVp };

// running mode
enum mode m = EXECVP;

int main(void) {

    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork() failed\n");
        exit(1);
    } else if (rc == 0) {
        switch (m) {

        case EXECL:
            printf("running execl: uses a list of arguments (l), doesn't use path:\n");

            // should fail, since it can't find "ls" without PATH
            if (execl("ls", "ls", "-a", NULL) == -1) {
                perror("expected failure");
            }

            if (execl("/bin/ls", "ls", "-a", NULL) == -1) {
                perror("execl"); // shouldn't fail anymore
            }
            break;

        case EXECLE:
            printf("running execle: uses a list of arguments (l) "
                   "and environment (e), doesn't use path:\n");

            // This shouldn't error, because I provide PATH through
            // env, but for some reason it doesn't work :(
            char* env[] = {"PATH=/usr/bin:/bin", NULL};
            if (execle("ls", "ls", "-a", NULL, env) == -1) {
                perror("execle");
            }
            break;

        case EXECLP:
            printf("running execlp: uses a list of arguments (l) and path(p):\n");
            if (execlp("ls", "ls", "-a", NULL) == -1) {
                perror("execl"); // shouldn't fail since it uses PATH to find `ls`
            }
            break;

        case EXECV:
            printf("running execv: uses array (v), but no PATH:\n");
            char* argsv[] = {"ls", "-a", NULL};
            if (execv(argsv[0], argsv) == -1) {
                perror("expected failure");
            }

            argsv[0] = "/bin/ls";
            if (execv(argsv[0], argsv) == -1) {
                perror("execv");
                exit(1);
            }
            break;

        case EXECVP:
            printf("running execvp: uses array (v), uses PATH (p):\n");
            char* argsvp[] = {"ls", "-a", NULL};
            if (execvp(argsvp[0], argsvp) == -1) {
                perror(argsvp[0]);
                exit(1);
            }
            break;

        case EXECVp:
            printf("running execvP: uses array (v) and provided path as a string:\n");
            char* argsvP[] = {"ls", "-a", NULL};
            char *path = "/usr/bin:/bin";
            if (execvP(argsvP[0], path, argsvP) == -1) {
                perror(argsvP[0]);
                exit(1);
            }
            break;
        }
    }
    return 0;
}
