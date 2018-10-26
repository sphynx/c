#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "hex.h"
#include "xor.h"

/*
 * Returns bitwise xor of two byte buffers of the same length. The
 * resulting byte buffer is malloc-ed, it's responsibility of the
 * caller to free it.
 */
uint8_t* xor(uint8_t* b1, uint8_t* b2, size_t len) {
    uint8_t* res = malloc(len);
    if (res == NULL) {
        printf("out of memory\n");
        return NULL;
    }

    for (unsigned int i = 0; i < len; i++) {
        res[i] = b1[i] ^ b2[i];
    }

    return res;
}

/*
 * Returns bitwise xor of two byte buffers of the same length. The
 * resulting byte buffer is malloc-ed, it's responsibility of the
 * caller to free it.
 */
uint8_t* xor_with_byte(uint8_t* buf, uint8_t key, size_t len) {
    uint8_t* res = malloc(len);
    if (res == NULL) {
        printf("out of memory\n");
        return NULL;
    }

    for (unsigned int i = 0; i < len; i++) {
        res[i] = buf[i] ^ key;
    }

    return res;
}


/*
 * Returns bitwise xor of two buffers represented as hex strings.
 * Result will also be encoded as a hex string. The resulting string
 * is malloc-ed, it's responsibility of the caller to free it.
 */
char* xor_hex(char* s1, char* s2) {
    assert(strlen(s1) == strlen(s2));

    size_t hex_len = strlen(s1);
    size_t bytes_len = hex_len / 2;

    uint8_t* b1 = read_hex(s1, hex_len);
    uint8_t* b2 = read_hex(s2, hex_len);
    uint8_t* res = xor(b1, b2, bytes_len);

    return write_hex(res, bytes_len);
}
