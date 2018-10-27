/*
 * Basic BASE64 encoder.
 * Implementation is based on https://en.wikipedia.org/wiki/Base64
 * Solution to http://cryptopals.com/sets/1/challenges/1
 * MIME version.
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
    size_t num_bytes;
    uint8_t* buffer;
    char* encoded_buffer;

    if (argc == 2) {
        stream = fopen(argv[1], "rb");
        if (stream == NULL) {
            perror(argv[1]);
            exit(1);
        }

        buffer = get_file_contents(stream, &num_bytes);
        encoded_buffer = base64_encode(buffer, num_bytes);

        if (puts(encoded_buffer) < 0) {
            printf("puts failed\n");
            exit(1);
        }

        free(buffer);
        free(encoded_buffer);
        fclose(stream);
    } else {
        printf("usage: base64 <file>\n");
        exit(1);
    }

    return 0;
}
