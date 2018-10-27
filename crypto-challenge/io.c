#include <assert.h>
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
