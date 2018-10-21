#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void) {

    printf("hello world (pid:%d)\n", (int) getpid());

    int fd = open("temp", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

    char str[] = "test";
    if (write(fd, str, strlen(str) + 1) < 0) {
        perror("write");
        exit(1);
    }

    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);

    } else if (rc == 0) {
        printf("child (pid:%d)\n", (int) getpid());
        printf("writing 'a' in child\n");

        char c = 'a';
        if (write(fd, &c, 1) < 0) {
            perror("write in child");
            exit(1);
        }

    } else {
        printf("parent of %d (pid:%d)\n", rc, (int) getpid());
        printf("writing 'b' in parent\n");

        char c = 'b';
        if (write(fd, &c, 1) < 0) {
            perror("write in parent");
            exit(1);
        }
    }

    return 0;
}

// The answer is: yes, they can both access the file descriptor and
// write to it simultaneously.
