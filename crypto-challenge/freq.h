#pragma once

#include <inttypes.h>
#include <stdio.h>

#define ALPHABET_SIZE 26

// Taken from https://en.wikipedia.org/wiki/Letter_frequency
#define FREQ_ORDER "etaoinshrdlcumwfgypbvkjxqz"

struct freq {
    char c;
    int freq;
};

/*
 * Returns a "score" of the string, i.e. some measurement of how
 * likely it is to be a human readable text. The lower score is the
 * better. The second argument is related to this particular algorithm
 * for scoring (you can consider it an abstraction leak), see below.
 * Input string is just a sequence of bytes, possibly not zero
 * terminated, so you have to use len to understand where it ends.
 */
int
score(uint8_t* str, size_t len, int n);


/*
 * Returns a best score for the string encoded in hex and makes
 * res_str to point to the best string. It's malloc-ed, so you have to
 * free it.
 */
int
best_score_hex(char* hex_str, char** res_str);

/*
 * Returns a best score for byte buffer and makes res_str to point to
 * the best string. It's malloc-ed, so you have to free it.
 */
int
best_score_raw(uint8_t* bytes, size_t bytes_len, char** res_str);
