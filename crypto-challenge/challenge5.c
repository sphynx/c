#include <string.h>

#include "xor.h"
#include "hex.h"

/*
 * Test driver for xor.c. Solves challenge #2.
 */
int
main(void) {
    uint8_t str[] =
        "Burning 'em, if you ain't quick and nimble\n"
        "I go crazy when I hear a cymbal";
    int msg_len = sizeof str - 1; // -1 because of \0 at the end

    uint8_t key[] = "ICE";
    int key_len = sizeof key - 1; // -1 because of \0 at the end

    uint8_t* xored = xor_with_repeating_key(str, msg_len, key, key_len);
    char* actu = write_hex(xored, msg_len);

    char* expe =
        "0b3637272a2b2e63622c2e69692a23693a2a3c6324202d623d63343c2a26226324272765272"
        "a282b2f20430a652e2c652a3124333a653e2b2027630c692b20283165286326302e27282f";

    if (strcmp(actu, expe) != 0) {
        printf("[FAIL]\n"
               "expected: %s\n"
               "actual:   %s\n",
               expe, actu);
        return 1;
    } else {
        printf("[OK] Challenge5 passed\n");
    }
    return 0;
}

