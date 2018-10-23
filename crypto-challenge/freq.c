#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "freq.h"

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

