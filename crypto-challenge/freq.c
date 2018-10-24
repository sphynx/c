#include <assert.h>
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freq.h"
#include "hex.h"
#include "xor.h"

static int
compare_by_freq(const void* x, const void* y) {
    const struct freq* sx = x;
    const struct freq* sy = y;
    return sy->freq - sx->freq; // descending order
}

static int
char_canonical_order(char c) {
    assert(c >= 'a' && c <= 'z');
    char canonical_order[] = FREQ_ORDER;
    char* cp = strchr(canonical_order, c);
    return cp - canonical_order;
}

/*
 * Returns a "score" of the string, i.e. some measurement of how
 * likely it is to be a human readable text. The lower score is the
 * better. The second argument is related to this particular algorithm
 * for scoring (you can consider it an abstraction leak), see below.
 */
int
score(char* str, int n) {

    /*
     * This particular scoring function is based on frequency of
     * English letters. Idea of this scoring is the following. We
     * count occurences of all letters and sort them based on
     * frequencies, producing a permutation of 26 English letters.
     * Then we add up the distances between positions of the top `n`
     * (say n=10) letters of our orders with the canonical order.
     */

    assert(str != NULL);
    assert(n >= 1 && n <= ALPHABET_SIZE);

    struct freq freqs[ALPHABET_SIZE];
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        struct freq f;
        f.c = 'a' + i;
        f.freq = 0;
        freqs[i] = f;
    }

    char c;
    while ((c = *str) != '\0') {
        if (c >= 'a' && c <= 'z') {
            freqs[c - 'a'].freq++;
        } else if (c >= 'A' && c <= 'Z') {
            freqs[c - 'A'].freq++;
        }
        str++;
    }

    qsort(freqs, ALPHABET_SIZE, sizeof(struct freq), compare_by_freq);

    int score = 0;
    for (int i = 0; i < n; i++) {
        score += abs(char_canonical_order(freqs[i].c) - i);
    }

    return score;
}

int best_score(char* hex_str, char** res_str) {
    size_t hex_len = strlen(hex_str);
    size_t bytes_len = hex_len / 2;

    uint8_t* bytes = read_hex(hex_str, hex_len);
    uint8_t* xor_mask = malloc(bytes_len);

    int min_score = INT_MAX;
    char* best_string;
    uint8_t cypher;

    uint8_t c = 0;
    for (;;) {
        // Prepare mask as a buffer filled with occurences of a single
        // character.
        memset(xor_mask, c, bytes_len);

        // Xor them.
        uint8_t* xored = xor(bytes, xor_mask, bytes_len);

        // FIXME: we can probably do it more efficiently (`realloc`?).
        // Convert xored buffer to a string (by copying and appending
        // zero byte).
        char* xored_str = malloc(bytes_len + 1);
        memcpy(xored_str, xored, bytes_len);
        xored_str[bytes_len] = '\0';
        free(xored);

        // Score the string and check if we improved.
        int curr_score = score(xored_str, 10);
        if (curr_score < min_score) {
            min_score = curr_score;
            cypher = c;
            best_string = xored_str;
            //printf("Best string so far (score = %4d): %s\n", min_score, best_string);
        } else {
            free(xored_str);
        }

        c++;
        if (c == 0) break;
    }

    *res_str = best_string;
    return min_score;
}
