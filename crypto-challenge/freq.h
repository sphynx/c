#pragma once

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
 */
int score(char* str, int n);


/*
 * Returns a best scrore for the string and makes res_str to point to
 * the best string. It's malloc-ed, so you have to free it.
 */

int best_score(char* hex_str, char** res_str);
