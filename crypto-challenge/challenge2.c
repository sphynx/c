#include <string.h>

#include "xor.h"

/*
 * Test driver for xor.c. Solves challenge #2.
 */
int
main(void) {
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
        printf("[OK] Challenge2 passed\n");
    }
    return 0;
}
