#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define BUFFER_SIZE 128

static void
cat_file(char* path) {
    FILE* fd = fopen(path, "r");
    if (fd) {
        char line[BUFFER_SIZE];

        while (fgets(line, BUFFER_SIZE, fd)) {
            printf("%s", line);
        }

        if (feof(fd)) {
            perror(path);
            exit(EXIT_FAILURE);
        }

        if (fclose(fd)) {
            perror(path);
            exit(EXIT_FAILURE);
        }
    } else {
        printf("my-cat: cannot open file\n");
        exit(EXIT_FAILURE);
    }
}

int
main(int argc, char* argv[]) {
    int i;
    for (i = 1; i < argc; i++) {
        cat_file(argv[i]);
    }

    return 0;
}
