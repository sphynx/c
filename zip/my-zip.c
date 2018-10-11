#include <stdio.h>
#include <stdlib.h>

static void
write_block(int len, char c, FILE* stream) {
    if (len == 0)
        return;

    // format and write run length as 4 bytes
    char bytes[4];
    bytes[0] = (len >> 24) & 0xFF;
    bytes[1] = (len >> 16) & 0xFF;
    bytes[2] = (len >>  8) & 0xFF;
    bytes[3] =  len        & 0xFF;
    fwrite(bytes, 4, 1, stream);

    // write the character
    fwrite(&c, 1, 1, stream);
}

static void
zip(FILE *stream) {
    int c;  // not `char` to accomodate EOF, which is -1
    int last_char = -2;
    int run_len = 0;

    while ((c = fgetc(stream)) != EOF) {
        if (c == last_char) {  // the current run is going on
            run_len++;
        } else {  // chars have changed, a new run is starting
            write_block(run_len, (char) last_char, stdout);
            last_char = c;
            run_len = 1;
        }
    }

    // write the last block, caused by EOF
    write_block(run_len, (char) last_char, stdout);
}

int
main(int argc, char* argv[]) {
    int i;
    FILE* f;

    if (argc <= 1) {
        printf("my-zip: file1 [file2 ...]\n");
        exit(1);
    }

    for (i = 1; i < argc; i++) {
        f = fopen(argv[i], "r");
        if (f == NULL) {
            perror(argv[i]);
            exit(1);
        } else {
            zip(f);
        }
        fclose(f);
    }

    return 0;
}
