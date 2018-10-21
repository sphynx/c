#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void) {

    int fd = open("temp", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        printf("hello\n");
        char c = '!';
        if (write(fd, &c, 1) < 0) {
            perror("write");
            exit(1);
        }
    } else {
        unsigned char c = 'v';

        int res;

        // Keep reading from the file until we've got a character or
        // an error, effectively blocking until child has wrote the
        // character into the same file.
        while ((res = pread(fd, &c, 1, 0) == 0))
            ;

        if (res < 0) {
            perror("read failed");
            exit(1);
        }

        printf("goodbye%c\n", c);
    }

    return 0;
}
