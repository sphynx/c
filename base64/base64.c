#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "base64.h"

typedef unsigned char sextet;

static size_t
base64_encoded_length(size_t len) {
    return (size_t) (4 * ceil(len / 3.0));
}

static char
encode_char(sextet sextet) {
    char res;

    assert(sextet < 64);

    if (sextet < 26) {
        // 0 .. 25 -> 'A' .. 'Z'
        res = 'A' + sextet;
    } else if (sextet < 52) {
        // 26 .. 51 -> 'a' .. 'z'
        res = 'a' + (sextet - 26);
    } else if (sextet < 62) {
        // 52 .. 61 -> '0' .. '9'
        res = '0' + (sextet - 52);
    } else if (sextet == 62) {
        // 62 -> '+'
        res = '+';
    } else {
        // 63 -> '/'
        res = '/';
    }
    return res;
}

static void
encode_1byte(uint8_t byte1, char* encoded) {
    sextet sextets[2];

    sextets[0] = byte1 >> 2;
    sextets[1] = (byte1 & 0x03) << 4;

    *encoded++ = encode_char(sextets[0]);
    *encoded++ = encode_char(sextets[1]);
    *encoded++ = '=';
    *encoded   = '=';
}

static void
encode_2bytes(uint8_t byte1, uint8_t byte2, char* encoded) {
    int i;
    sextet sextets[3];

    sextets[0] = byte1 >> 2;
    sextets[1] = ((byte1 & 0x03) << 4) | (byte2 >> 4);
    sextets[2] = (byte2 & 0x0F) << 2;

    for (i = 0; i < 3; i++) {
        *encoded++ = encode_char(sextets[i]);
    }
    *encoded = '=';
}

static void
encode_3bytes(uint8_t byte1, uint8_t byte2, uint8_t byte3, char* encoded) {
    int i;
    sextet sextets[4];

    sextets[0] = byte1 >> 2;
    sextets[1] = ((byte1 & 0x03) << 4) | (byte2 >> 4);
    sextets[2] = ((byte2 & 0x0F) << 2) | (byte3 >> 6);
    sextets[3] = byte3 & 0x3F;

    for (i = 0; i < 4; i++) {
        *encoded++ = encode_char(sextets[i]);
    }
}


/*
 * Encodes to BASE64, returns a pointer to a newly allocated
 * zero-terminated string or NULL in case of error. Don't forget to
 * free() that string later.
 */
char*
base64_encode(uint8_t* from, size_t len) {
    unsigned long i;
    long padded_bytes;

    size_t to_len;
    char* to;
    char* curr;

    // First, determine size of required result buffer, then malloc
    // it (with 1 extra byte for zero termination).
    to_len = base64_encoded_length(len);
    to = (char*) malloc(to_len + 1);
    if (to == NULL) {
        return NULL;
    }

    // Now we handle 3 bytes at a time, converting them into 4
    // sextets.
    curr = to;
    for (i = 0; i < len / 3; i++) {
        encode_3bytes(*from, *(from + 1), *(from + 2), curr);
        from += 3;
        curr += 4;
    }

    // Convert leftover bytes (if any) using padding.
    padded_bytes = len - 3 * (len / 3);
    if (padded_bytes == 1) {
        encode_1byte(*from, curr);
    } else if (padded_bytes == 2) {
        encode_2bytes(*from, *(from + 1), curr);
    }

    // Zero-terminate.
    to[to_len] = '\0';

    return to;
}
