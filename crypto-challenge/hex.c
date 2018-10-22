#include <assert.h>
#include <stdlib.h>

#include "hex.h"

static uint8_t
char_to_four_bits(char c) {
    uint8_t res;
    if (c >= '0' && c <= '9') {
        res = c - '0';
    } else if (c >= 'a' && c <= 'f') {
        res = c - 'a' + 10;
    } else if (c >= 'A' && c <= 'F') {
        res = c - 'A' + 10;
    } else {
        fprintf(stderr, "to_byte: unrecognized hex char: %c\n", c);
        res = 16;
    }
    return res;
}

static char
four_bits_to_char(uint8_t b) {
    assert(b < 16);
    return "0123456789abcdef"[b];
}

uint8_t*
read_hex(char* hex_str, size_t hex_len) {
    assert(hex_len % 2 == 0);

    size_t bytes_len = hex_len / 2;
    uint8_t* bytes = malloc(bytes_len);
    if (bytes == NULL) {
        return NULL;
    }

    for (unsigned long i = 0; i < bytes_len; i++) {
        char c1 = hex_str[2*i];
        char c2 = hex_str[2*i + 1];
        uint8_t hi = char_to_four_bits(c1);
        uint8_t lo = char_to_four_bits(c2);
        if (hi == 16 || lo == 16) { // error
            return NULL;
        }

        bytes[i] = hi << 4 | lo;
    }


    return bytes;
}

char*
write_hex(uint8_t* bytes, size_t bytes_len) {
    size_t hex_len = bytes_len * 2 + 1; // +1 for '\0'
    char* hex = malloc(hex_len);
    if (hex == NULL) {
        return NULL;
    }

    for (unsigned long i = 0; i < bytes_len; i++) {
        uint8_t hi = bytes[i] >> 4;
        uint8_t lo = bytes[i] & 0x0f;
        hex[2*i]   = four_bits_to_char(hi);
        hex[2*i+1] = four_bits_to_char(lo);
    }

    hex[hex_len - 1] = '\0';
    return hex;
}
