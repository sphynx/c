#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/*
 * Here we use `waitpid()` instead of `wait()`. It might be useful
 * when we have several children and want to wait for a particular
 * one.
 */

int main(void) {

    int rc = fork();

    if (rc < 0) {
        perror("fork");
        exit(1);
    } else if (rc == 0) {
        printf("child (pid:%d)\n", (int) getpid());
    } else {
        int status;
        int wpid = waitpid(rc, &status, 0);

        if (wpid == -1) {
            perror("waitpid");
            exit(1);
        }

        if (WIFEXITED(status)) {
            printf("child (pid:%d) exited, status=%d\n",
                   wpid, WEXITSTATUS(status));
        }
        if (WIFSIGNALED(status)) {
            printf("child (pid:%d) signaled (signal:%d)\n",
                   wpid, WTERMSIG(status));
        }
        if (WIFSTOPPED(status)) {
            printf("child (pid:%d) stopped\n", wpid);
        }

        for (int i = 0; i < 20000; i++) {
            printf("parent\n");
        }
    }

    return 0;
}
