#pragma once

#include <stdio.h>
#include <inttypes.h>

/*
 * Takes two NUL-terminated strings of the same length. Returns a
 * number of differing bits.
 */
size_t hamming_distance(char* s1, char* s2);

/*
 * Takes two byte arrays of the same length and their length. Returns
 * a number of differing bits.
 */
size_t hamming_distance_bytes(uint8_t* b1, uint8_t* b2, size_t len);

/*
 * Returns number of 1-bits set in a byte, so called "population count".
 */
size_t pop_count(uint8_t byte);
