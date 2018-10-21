#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/*
 * We can use `make run | grep -n child` with the parent printing many
 * messages to observe that the parent does not wait for the child (we
 * will see different results):

 * ➜  process-api git:(master) ✗ make run | grep child -n
 * 2342:parechild
 * ➜  process-api git:(master) ✗ make run | grep child -n
 * 2:child

 * So we can use `wait()` and fix that.
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
        int wpid = wait(&status);

        if (wpid == -1) {
            perror("wait");
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
