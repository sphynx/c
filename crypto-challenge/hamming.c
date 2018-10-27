#include <assert.h>
#include <inttypes.h>
#include <string.h>

#include "hamming.h"

// Precalculated number of 1-bits in corresponding bytes from 0
// (00000000 -> 0) to 255 (11111111 -> 8). Formatted as 16x16 table.
static size_t table[] =
// 0 1 2 3 4 5 6 7 8 9 A B C D E F
  {0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,  // 0x0
   1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,  // 0x1
   1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,  // 0x2
   2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,  // 0x3
   1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,  // 0x4
   2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,  // 0x5
   2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,  // 0x6
   3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,  // 0x7
   1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,  // 0x8
   2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,  // 0x9
   2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,  // 0xA
   3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,  // 0xB
   2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,  // 0xC
   3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,  // 0xD
   3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,  // 0xE
   4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8}; // 0xF

// Precalculated table version. See more details about this function
// here: https://en.wikipedia.org/wiki/Hamming_weight and in "Hacker's
// delight" book, there are lots of tricks to count it in a very
// efficient manner. I just use a precalculated table (precalculated
// with a naive approach by `precalculate-pop-count`).
size_t
pop_count(uint8_t byte) {
    return table[byte];
}

size_t
hamming_distance(char* s1, char* s2) {
    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);
    assert(len1 == len2);

    size_t distance = 0;
    for (unsigned long i = 0; i < len1; i++) {
        distance += pop_count(s1[i] ^ s2[i]);
    }

    return distance;
}

size_t
hamming_distance_bytes(uint8_t* b1, uint8_t* b2, size_t len) {
    size_t distance = 0;
    for (unsigned long i = 0; i < len; i++) {
        distance += pop_count(b1[i] ^ b2[i]);
    }

    return distance;
}
