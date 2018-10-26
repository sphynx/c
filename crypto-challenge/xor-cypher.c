/*
 * Solves challenge #3 from the first set.
 * http://cryptopals.com/sets/1/challenges/3
 */

#include "freq.h"
#include "hex.h"
#include "xor.h"

int
main(void) {
    char* str = "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736";
    char* result;
    int min_score = best_score_hex(str, &result);
    printf("Final best string (score=%d): %s\n", min_score, result);
    return 0;
}

