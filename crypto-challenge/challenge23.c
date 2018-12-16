#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mersenne_twister.h"

#define STATE_SIZE 624
#define TEST_SIZE   10

#define U 11UL
#define S 7UL
#define B 0x9D2C5680UL
#define T 15UL
#define C 0xEFC60000UL
#define L 18UL

static uint32_t untemper(uint32_t y)
{
    y ^= y >> L;
    y ^= (y << T) & C;

    // Ugly bit-by-bit solution for 7-bits shift, but for now I'm not
    // sure how to improve it.
    y ^= (((((y << 7) ^ (y << 14)) & (1UL << 31)) |
         (((y << 7) ^ (y << 14) ^ (y << 21) ^ (y << 28)) & (1UL << 28)) |
         ((y << 7) & (1UL << 27)) |
         (((y << 7) ^ (y << 14) ^ (y << 21)) & (1UL << 26)) |
         ((y << 7) & (1UL << 24)) |
         (((y << 7) ^ (y << 14) ^ (y << 21)) & (1UL << 21)) |
         (((y << 7) ^ (y << 14)) & (1UL << 19)) |
         ((y << 7) & (1UL << 18)) |
         (((y << 7) ^ (y << 14)) & (1UL << 14)) |
         ((y << 7) & (1UL << 12)) |
         ((y << 7) & (1UL << 10)) |
         ((y << 7) & (1UL << 9)) |
         ((y << 7) & (1UL << 7))) & B);

    y ^= (y >> U) ^ (y >> (2 * U));
    return y;
}

int main(void)
{
    uint32_t outs[STATE_SIZE];
    uint32_t state[STATE_SIZE];
    uint32_t old[TEST_SIZE];
    uint32_t new[TEST_SIZE];

    seed(arc4random());
    for (int i = 0; i < STATE_SIZE; i++) {
        outs[i] = extract();
        state[i] = untemper(outs[i]);
    }

    // Now we have reconstructed MT state, so we can predict further
    // values.

    // Random numbers from original RNG:
    for (int i = 0; i < TEST_SIZE; i++)
        old[i] = extract();

    // Now, set up a new RNG:
    set_state(state);

    // And get the random number from this copied RNG:
    for (int i = 0; i < TEST_SIZE; i++)
        new[i] = extract();

    for (int i = 0; i < TEST_SIZE; i++)
        printf("old: %10" PRIu32 ", new: %10" PRIu32 "\n",
               old[i], new[i]);

    if (memcmp(old, new, sizeof(old)) == 0) {
        printf("Test passed: RNG state reconstructed\n");
        exit(EXIT_SUCCESS);
    } else {
        printf("Test failed\n");
        exit(EXIT_FAILURE);
    }
}
