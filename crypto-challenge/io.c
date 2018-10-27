#include <assert.h>
#include <ctype.h>
#include <stdlib.h>

#include "io.h"

uint8_t*
get_file_contents(FILE* stream, size_t* len) {

    assert(len != NULL && stream != NULL);

    size_t num_bytes;
    uint8_t* buffer;

    // seek to end to determine file size
    if (fseek(stream, 0, SEEK_END) == -1) {
        perror("fseek");
        return NULL;
    }

    long pos = ftell(stream);
    if (pos == -1) {
        perror("ftell");
        return NULL;
    } else {
        num_bytes = (size_t) pos;
    }

    // rewind back to the beginning
    if (fseek(stream, 0, SEEK_SET) == -1) {
        perror("fseek");
        return NULL;
    }

    buffer = malloc(num_bytes);
    if (buffer == NULL) {
        perror("malloc");
        return NULL;
    }

    if (fread(buffer, 1, num_bytes, stream) != num_bytes) {
        fprintf(stderr, "fread failed\n");
        return NULL;
    }

    *len = num_bytes;
    return buffer;
}

#define START_SIZE 512

// 2^24 = 16 MB is the max we support
#define MAX_BUF_SIZE (1UL << 24)

char*
read_non_space(FILE* stream) {
    int c;

    char* str = malloc(START_SIZE); // this means we have START_SIZE-1 chars
    size_t remaining_chars = START_SIZE - 1;

    int ix = 0;
    size_t buf_size = START_SIZE;

    for (;;) {
        c = fgetc(stream);

        if (feof(stream)) {
            break;
        }

        if (ferror(stream)) {
            perror("fgetc");
            return NULL;
        }

        if (isspace(c)) {
            continue;
        }

        if (remaining_chars == 0) {
            // FIXME: check that it's not too big
            remaining_chars = buf_size - 1;

            if (buf_size < MAX_BUF_SIZE) {
                buf_size *= 2;
            } else {
                fprintf(stderr, "input is too big: more than %ld bytes\n",
                        MAX_BUF_SIZE);
                return NULL;
            }
            str = realloc(str, buf_size);
        }

        // append char to the buffer
        str[ix++] = c;
        remaining_chars--;
    }

    // We should have allocated enough space to append NUL-byte.
    str[ix] = '\0';

    return str;
}
