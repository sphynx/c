/*
 * Solves challenge #3 from the first set.
 * http://cryptopals.com/sets/1/challenges/3
 */

#include <string.h>

#include "freq.h"
#include "hex.h"
#include "xor.h"

int
main(void) {
    char* str = "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736";

    char* actu;
    (void) best_score_hex(str, &actu);

    char* expe = "Cooking MC's like a pound of bacon";

    if (strcmp(actu, expe) != 0) {
        printf("[FAIL]\n"
               "expected: %s\n"
               "actual:   %s\n",
               expe, actu);
        return 1;
    } else {
        printf("[OK] Challenge3 passed\n");
    }
    return 0;
}

