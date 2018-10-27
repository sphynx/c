/*
 * A tool to decode BASE64.
 */

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "base64.h"
#include "io.h"

int
main(int argc, char* argv[]) {

    FILE* stream;
    char* str;
    uint8_t* decoded;
    size_t byte_len;

    if (argc == 2) {
        stream = fopen(argv[1], "rb");
        if (stream == NULL) {
            perror(argv[1]);
            exit(1);
        }

        str = read_non_space(stream);
        if (str == NULL) {
            printf("read_non_space failed\n");
            exit(1);
        }

        decoded = base64_decode(str, &byte_len);
        if (decoded == NULL) {
            printf("base64_decode failed\n");
            exit(1);
        }

        if (fwrite(decoded, 1, byte_len, stdout) != byte_len) {
            printf("fwrite failed\n");
            exit(1);
        }

        free(str);
        free(decoded);
        fclose(stream);
    } else {
        printf("usage: %s <file>\n", argv[0]);
        exit(1);
    }

    return 0;
}
