/*
 * Based on pseudo-code from
 * https://en.wikipedia.org/wiki/Mersenne_Twister
 *
 * MT19937 version (32-bits word).
 *
 */

#include "mersenne_twister.h"

// MT algorithm constants.
#define w 32UL
#define n 624UL
#define m 397UL
#define r 31UL
#define a 0x9908B0DFUL
#define u 11UL
#define d 0xFFFFFFFFUL
#define s 7UL
#define b 0x9D2C5680UL
#define t 15UL
#define c 0xEFC60000UL
#define l 18UL

// Initialization constant.
#define f 1812433253UL

// Derived constants.
#define lower_mask ((1UL << r) - 1)
#define upper_mask (~lower_mask)

// Global state.
static uint32_t MT[n];
static uint32_t index = n + 1;

// Generate the next `n` values.
static void twist(void) {
    for (uint32_t i = 0; i < n; i++) {
        uint32_t x = (MT[i] & upper_mask) + (MT[(i + 1) % n] & lower_mask);
        uint32_t xA = x >> 1;
        if ((x % 2) != 0)
            xA ^= a;

        MT[i] = MT[(i + m) % n] ^ xA;
    }
    index = 0;
}

void seed(uint32_t seed)
{
    index = n;
    MT[0] = seed;
    for (uint32_t i = 1; i < n; i++)
        MT[i] = f * (MT[i - 1] ^ (MT[i - 1] >> (w - 2))) + i;
}

uint32_t extract(void) {
    if (index >= n) {
        if (index > n)
            seed(5489);
        twist();
    }

    uint32_t y = MT[index++];
    y ^= (y >> u) & d;
    y ^= (y << s) & b;
    y ^= (y << t) & c;
    y ^= y >> l;

    return y;
}
