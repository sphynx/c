#include <assert.h>
#include <stdlib.h>

#include "hex.h"

static uint8_t
to_byte(char c) {
    uint8_t res;
    if (c >= '0' && c <= 9) {
        res = c - '0';
    } else if (c >= 'a' && c <= 'f') {
        res = c - 'a' + 10;
    } else if (c >= 'A' && c <= 'F') {
        res = c - 'A' + 10;
    } else {
        fprintf(stderr, "unrecognized byte: %c", c);
        res = 16;
    }
    return res;
}

uint8_t*
read_hex(char *hex_str, size_t len) {
    char b1, b2;
    assert(len % 2 == 0);

    uint8_t* res = malloc(len/2);
    uint8_t* curr = res;
    if (res == NULL) {
        return NULL;
    }

    for (unsigned int i = 0; i < len/2; i++) {
        b1 = *hex_str++;
        b2 = *hex_str++;
        *curr++ = to_byte(b1) << 4 | to_byte(b2);
    }
    return res;
}
