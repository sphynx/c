#pragma once

/*
 * Takes two NUL-terminated strings of the same length. Returns a
 * number of differing bits.
 */
size_t hamming_distance(char* s1, char* s2);


/*
 * Returns number of 1-bits set in a byte, so called "population count".
 */
size_t pop_count(uint8_t byte);
