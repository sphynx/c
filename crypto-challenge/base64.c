#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "base64.h"

typedef uint8_t sextet;

/*
 * ********************************* ENCODING ********************************
 */

static size_t
base64_encoded_length(size_t bytes_len) {
    return (size_t) (4 * ceil(bytes_len / 3.0));
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



/*
 * ********************************* DECODING ********************************
 */

#define DECODE_WRONG_CHAR 64

static uint8_t
decode_char(char c) {
    uint8_t res;

    assert(isalnum(c) || c == '+' || c == '/');

    if (isupper(c)) {
        res = c - 'A';
    } else if (islower(c)) {
        res = c - 'a' + 26;
    } else if (isdigit(c)) {
        res = c - '0' + 52;
    } else if (c == '+') {
        res = 62;
    } else if (c == '/') {
        res = 63;
    } else {
        fprintf(stderr, "unsupported char: %c\n", c);
        res = DECODE_WRONG_CHAR;
    }

    return res;
}


static void
decode_step_1byte(char* from, uint8_t* to) {
    // read and decode 2 sextets
    uint8_t sextet1 = decode_char(*from);
    uint8_t sextet2 = decode_char(*(from + 1));

    // check for errors
    assert(sextet1 != DECODE_WRONG_CHAR);
    assert(sextet2 != DECODE_WRONG_CHAR);

    // now combine 2 sextets (2*6 bits) into a single byte
    uint8_t byte1 = (sextet1 << 2) | (sextet2 >> 4);

    // write 1 byte
    *to++ = byte1;
}

static void
decode_step_2bytes(char* from, uint8_t* to) {
    // read and decode 3 sextets
    uint8_t sextet1 = decode_char(*from);
    uint8_t sextet2 = decode_char(*(from + 1));
    uint8_t sextet3 = decode_char(*(from + 2));

    // check for errors
    assert(sextet1 != DECODE_WRONG_CHAR);
    assert(sextet2 != DECODE_WRONG_CHAR);
    assert(sextet3 != DECODE_WRONG_CHAR);

    // now combine 3 sextets (3*6 bits) into 2 bytes
    uint8_t byte1 = (sextet1 << 2) | (sextet2 >> 4);
    uint8_t byte2 = (sextet2 << 4) | (sextet3 >> 2);

    // write 2 bytes
    *to++ = byte1;
    *to++ = byte2;
}

static void
decode_step_3bytes(char* from, uint8_t* to) {
    // read and decode 4 chars into 4 sextets
    uint8_t sextet1 = decode_char(*from);
    uint8_t sextet2 = decode_char(*(from + 1));
    uint8_t sextet3 = decode_char(*(from + 2));
    uint8_t sextet4 = decode_char(*(from + 3));

    // check for errors
    assert(sextet1 != DECODE_WRONG_CHAR);
    assert(sextet2 != DECODE_WRONG_CHAR);
    assert(sextet3 != DECODE_WRONG_CHAR);
    assert(sextet4 != DECODE_WRONG_CHAR);

    // now combine 4 sextets (4*6 bits) into 3 bytes (3*8)
    uint8_t byte1 = (sextet1 << 2) | (sextet2 >> 4);
    uint8_t byte2 = (sextet2 << 4) | (sextet3 >> 2);
    uint8_t byte3 = (sextet3 << 6) | sextet4;

    // write 3 bytes
    *to++ = byte1;
    *to++ = byte2;
    *to++ = byte3;
}

uint8_t*
base64_decode(char* from, size_t* len) {
    assert(from != NULL && len != NULL);

    // FIXME: we should clean all the whitespace first

    size_t text_len = strlen(from);

    // we assume it is properly padded, so we always have 4-character blocks
    assert(text_len % 4 == 0);

    // analyze padding
    char last     = from[text_len - 1];
    char pre_last = from[text_len - 2];

    int padded_block_len = 0;
    if (last == '=') {
        padded_block_len = (pre_last == '=') ? 1 : 2;
    }

    // determine the size of decoded bytes
    size_t bytes_len = text_len / 4 * 3 + padded_block_len;

    uint8_t* to = malloc(bytes_len);
    if (to == NULL) {
        perror("malloc");
        return NULL;
    }

    // read 4-bytes and write 3-bytes on each step
    unsigned long i;
    for (i = 0; i < text_len / 4 - 1; i++) {
        decode_step_3bytes(&from[4 * i], &to[3 * i]);
    }

    // handle the last block now (possibly with paddin)
    if (padded_block_len == 0) {
        // just go on as normal, no padding
        decode_step_3bytes(&from[4 * i], &to[3 * i]);
    } else if (padded_block_len == 1) {
        decode_step_1byte(&from[4 * i], &to[3 * i]);
    } else if (padded_block_len == 2) {
        decode_step_2bytes(&from[4 * i], &to[3 * i]);
    }

    *len = bytes_len;
    return to;
}
