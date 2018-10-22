#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hex.h"

/*
 * Returns bitwise xor of two buffers represented as hex strings.
 * Result will also be encoded as a hex string. The resulting string
 * is malloc-ed, it's responsibility of the caller to free it.
 */
char* xor_hex(char* s1, char* s2) {
    assert(strlen(s1) == strlen(s2));
    size_t hex_len = strlen(s1);
    size_t byte_len = hex_len / 2;

    uint8_t* b1 = read_hex(s1, hex_len);
    uint8_t* b2 = read_hex(s2, hex_len);

    uint8_t* re = malloc(byte_len);
    if (re == NULL) {
        printf("out of memory\n");
        return NULL;
    }

    for (unsigned int i = 0; i < byte_len; i++) {
        re[i] = b1[i] ^ b2[i];
    }

    return write_hex(re, byte_len);
}

/*
 * Test driver.
 */
int main(void) {
    char* str1 = "1c0111001f010100061a024b53535009181c";
    char* str2 = "686974207468652062756c6c277320657965";
    char* actu = xor_hex(str1, str2);
    char* expe = "746865206b696420646f6e277420706c6179";

    if (strcmp(actu, expe) != 0) {
        printf("[FAIL]\n"
               "expected: %s\n"
               "actual:   %s\n",
               expe, actu);
        return 1;
    } else {
        printf("[OK] xor test passed\n");
    }
    return 0;
}
