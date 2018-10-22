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

#include "hex.h"
#include "base64.h"

int
main(int argc, char* argv[]) {
    FILE* stream;

    char* hex_buffer = NULL;
    size_t hex_buffer_cap;
    ssize_t num_chars;

    uint8_t* buffer = NULL;
    size_t buffer_size;

    char* encoded_buffer;

    if (argc == 2) {
        stream = fopen(argv[1], "rb");
        if (stream == NULL) {
            perror(argv[1]);
            exit(1);
        }

        num_chars = getline(&hex_buffer, &hex_buffer_cap, stream);
        if (num_chars == -1) {
            perror("getline failed");
            exit(1);
        }

        buffer = read_hex(hex_buffer, num_chars - 1); // drop newline char
        buffer_size = (num_chars - 1) / 2;
        encoded_buffer = base64_encode(buffer, buffer_size);
        if (puts(encoded_buffer) < 0) {
            printf("puts failed\n");
            exit(1);
        }

        free(hex_buffer);
        free(buffer);
        free(encoded_buffer);
        fclose(stream);
    } else {
        fprintf(stderr, "usage: %s <file>\n", argv[0]);
        exit(1);
    }

    return 0;
}
