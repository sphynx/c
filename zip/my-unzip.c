#include <stdio.h>
#include <stdlib.h>

static void
print_block(char bytes[], char c) {
    int i;

    // reconstruct the integer
    int len =  (((int) bytes[0]) << 24)
             | (((int) bytes[1]) << 16)
             | (((int) bytes[2]) << 8)
             | ((int)  bytes[3]);

    for (i = 0; i < len; i++) {
        printf("%c", c);
    }
}

static void
unzip(FILE *stream, char* name) {
    char bytes[4];
    char c;
    size_t read_objects;

    while ((read_objects = fread(bytes, 1, 4, stream)) != 0) {
        if (read_objects == 4) {
            read_objects = fread(&c, 1, 1, stream);
            if (read_objects == 1) {
                print_block(bytes, c);
            } else {
                printf("malformed output\n");
                exit(1);
            }
        } else {
            printf("malformed output\n");
            exit(1);
        }
    }

    if (ferror(stream)) {
        perror(name);
        exit(1);
    }
}

int
main(int argc, char* argv[]) {
    int i;
    FILE* f;

    if (argc <= 1) {
        printf("my-unzip: file1 [file2 ...]\n");
        exit(1);
    }

    for (i = 1; i < argc; i++) {
        f = fopen(argv[i], "rb");
        if (f == NULL) {
            perror(argv[i]);
            exit(1);
        } else {
            unzip(f, argv[i]);
        }
        fclose(f);
    }

    return 0;
}
