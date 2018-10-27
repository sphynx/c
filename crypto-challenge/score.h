#pragma once

#include <inttypes.h>
#include <stdio.h>

#define ALPHABET_SIZE 26

// Taken from https://en.wikipedia.org/wiki/Letter_frequency
#define FREQ_ORDER " etaoinshrdlcumwfgypbvkjxqz"

struct freq {
    char c;
    int freq;
};

/*
 * Returns a best score for the string encoded in hex and makes
 * `res_str` to point to the best string. It's malloc-ed, so you have
 * to free it. "Score" is a certain measurement of how likely the
 * input string is to be a human readable text. The lower score is the
 * better.
 */
int
best_score_hex(char* hex_str, char** res_str);

/*
 * Returns a best score for byte buffer and makes res_str to point to
 * the best string. It's malloc-ed, so you have to free it.
 */
int
best_score_raw(uint8_t* bytes, size_t bytes_len, char** res_str);

/*
 * Returns the key corresponding to the best score for byte buffer
 * when only every `step`-th byte starting from byte `start` is taken
 * into account.
 */
uint8_t
best_key_step(uint8_t* bytes, size_t bytes_len, size_t start, size_t step);
