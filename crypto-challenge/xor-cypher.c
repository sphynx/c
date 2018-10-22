/*
 * Solves challenge #3 from the first set.
 * http://cryptopals.com/sets/1/challenges/3
 */

#include <assert.h>
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xor.h"
#include "hex.h"

#define ALPHABET_SIZE 26

// Taken from https://en.wikipedia.org/wiki/Letter_frequency
#define FREQ_ORDER "etaoinshrdlcumwfgypbvkjxqz"

struct freq {
    char c;
    int freq;
};

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

int
score1(char* str, int n) {

    assert(str != NULL);
    assert(n >= 1 && n <= ALPHABET_SIZE);

    // Idea of this scoring is the following. We count occurences of
    // all letters and sort them based on frequencies, producing a
    // permutation of 26 English letters. Then we add up the distances
    // between positions of the top `n` (say n=10) letters of our
    // orders with the canonical order. The lower score is the better.

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
        int score = score1(xored_str, 10);
        if (score < min_score) {
            min_score = score;
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

