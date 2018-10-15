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

        // seek to end to determine file size
        if (fseek(stream, 0, SEEK_END) == -1) {
            perror("fseek");
            exit(1);
        }

        num_bytes = ftell(stream);

        // rewind back to the beginning
        if (fseek(stream, 0, SEEK_SET) == -1) {
            perror("fseek");
            exit(1);
        }

        // allocate memory, encode and print the result
        buffer = (uint8_t*) malloc(num_bytes);
        if (buffer == NULL) {
            exit(1);
        }

        if (fread(buffer, 1, num_bytes, stream) != num_bytes) {
            printf("fread failed\n");
            exit(1);
        }

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
