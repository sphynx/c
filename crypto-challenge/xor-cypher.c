/*
 * Solves challenge #3 from the first set.
 * http://cryptopals.com/sets/1/challenges/3
 */

#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freq.h"
#include "hex.h"
#include "xor.h"

int
main(void) {
    char* str =
        "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736";

    size_t hex_len = strlen(str);
    size_t bytes_len = hex_len / 2;

    uint8_t* bytes = read_hex(str, hex_len);
    uint8_t* mask = malloc(bytes_len);

    int min_score = INT_MAX;
    char* best_string;
    char cypher;

    for (int i = 0; i < ALPHABET_SIZE; i++) {

        // Prepare mask as a buffer filled with occurences of a single
        // character.
        char c = 'A' + i;
        memset(mask, c, bytes_len);

        // Xor them.
        uint8_t* xored = xor(bytes, mask, bytes_len);

        // Convert xored buffer to a string (copying and appending
        // zero byte).
        char* xored_str = malloc(bytes_len + 1);
        memcpy(xored_str, xored, bytes_len);
        xored_str[bytes_len] = '\0';

        // Score the string and check if we improved.
        int curr_score = score(xored_str, 10);
        if (curr_score < min_score) {
            min_score = curr_score;
            cypher = c;
            best_string = strdup(xored_str);
            printf("Best string so far (score = %4d): %s\n",
                   min_score, best_string);
        }

        free(xored);
        free(xored_str);
    }

    printf("\n");
    printf("Final best string (score=%d, cypher=%c): %s\n",
           min_score, cypher, best_string);

    return 0;
}

