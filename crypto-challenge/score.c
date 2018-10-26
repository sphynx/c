#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "score.h"
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
    assert((c >= 'a' && c <= 'z') || c == ' ');
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
static int
score(uint8_t* str, size_t len, int n) {

    /*
     * This particular scoring function is based on frequency of
     * English letters. Idea of this scoring is the following. We
     * count occurences of all letters and sort them based on
     * frequencies, producing a permutation of 26 English letters.
     * Then we add up the distances between positions of the top `n`
     * (say n=10) letters of our orders with the canonical order.
     */

    assert(str != NULL);
    assert(n >= 1 && n <= ALPHABET_SIZE + 1);

    struct freq freqs[ALPHABET_SIZE];
    for (int i = 0; i < ALPHABET_SIZE + 1; i++) {
        struct freq f;

        if (i == ALPHABET_SIZE) {
            f.c = ' ';
        } else {
            f.c = 'a' + i;
        }
        f.freq = 0;
        freqs[i] = f;
    }

    int score = 0;

    for (unsigned int i = 0; i < len; i++) {
        char c = str[i];
        if (c >= 'a' && c <= 'z') {
            freqs[c - 'a'].freq++;
        } else if (c >= 'A' && c <= 'Z') {
            freqs[c - 'A'].freq++;
        } else if (c == ' ') {
            freqs[ALPHABET_SIZE].freq++;
        } else if (!ispunct(c)) {
            score += n; // penalty for non-punctuation chars
        }
    }

    qsort(freqs, ALPHABET_SIZE + 1, sizeof(struct freq), compare_by_freq);

    for (int i = 0; i < n; i++) {
        score += abs(char_canonical_order(freqs[i].c) - i);
    }

    return score;
}

int
best_score_hex(char* hex_str, char** res_str) {
    size_t hex_len = strlen(hex_str);
    size_t bytes_len = hex_len / 2;
    uint8_t* bytes = read_hex(hex_str, hex_len);
    return best_score_raw(bytes, bytes_len, res_str);
}

int
best_score_raw(uint8_t* bytes, size_t bytes_len, char** res_str) {
    int min_score = INT_MAX;
    uint8_t* best_string;
    uint8_t cypher;

    uint8_t key = 0;
    for (;;) {
        // Xor them with the key.
        uint8_t* xored = xor_with_byte(bytes, key, bytes_len);

        // Score the string and check if we improved.
        int curr_score = score(xored, bytes_len, 20);
        if (curr_score < min_score) {
            min_score = curr_score;
            cypher = key;
            best_string = xored;
        } else {
            free(xored);
        }

        key++;
        if (key == 0) break; // full circle: we got to 0 again
    }

    char* best_str_terminated = malloc(bytes_len + 1);
    memcpy(best_str_terminated, best_string, bytes_len);
    best_str_terminated[bytes_len] = '\0';
    free(best_string);

    *res_str = best_str_terminated;
    return min_score;
}
