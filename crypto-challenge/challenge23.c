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

/*
 This in what we need to undo:

    y ^= (y >> U);
    y ^= (y << S) & B;
    y ^= (y << T) & C;
    y ^= (y >> L);


The first step of undoing will be reversing the last transformation:

    y = y ^ (y >> 18);

    y00 y01 y02 y03 y04 y05 y06 y07 y08 y09 y10 y11 y12 y13 y14 y15 y16 y17 y18 y19 y20 y21 y22 y23 y24 y25 y26 y27 y28 y29 y30 y31  : y
      0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0 y00 y01 y02 y03 y04 y05 y06 y07 y08 y09 y10 y11 y12 y13  : y >> 18

    y00                                                             y16 y17 y[i]^y[i+18] (for i = 0 .. 13)

     0                                                                      y00           y01


    Now, invert:

    for y ^ (y >> 18) (it will be the same)


   ((y ^ (y >> 18)) >> 18) ^ (y ^ (y >> 18))
   ((y ^ (y >> 18)) >> 18) ^ y ^ (y >> 18)



    y00 y01 y02 y03 y04 y05 y06 y07 y08 y09 y10 y11 y12 y13 y14 y15 y16 y17 y18 y19 y20 y21 y22 y23 y24 y25 y26 y27 y28 y29 y30 y31  : y
      0   0   0   0   0   0   0   0   0   0   0 y00 y01 y02 y03 y04 y05 y06 y07 y08 y09 y10 y11 y12 y13 y14 y15 y16 y17 y18 y19 y20: y >> 11

     y00 y01                                    y11^y00 y12^y01  y[i] ^ y[i+11] for i = 0 .. 20

     y00 y01                                    y11^y00 y12^y01  y[i] ^ y[i+11] | y10^y21 y11^y22
     0    0                                     y00     y01      y02              y10     y11^y0

                                                                                          y0^22



    y = y ^ ((y << 15) & 0xEFC60000)

    y00 y01 y02 y03 y04 y05 y06 y07 y08 y09 y10 y11 y12 y13 y14 y15 y16 y17 y18 y19 y20 y21 y22 y23 y24 y25 y26 y27 y28 y29 y30 y31  : y

    y15 y16 y17 y18 y19 y20 y21 y22 y23 y24 y25 y26 y27 y28 y29 y30 y31   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  : y << 15
      1   1   1   0   1   1   1   1   1   1   0   0   0   1   1   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  : 0xEFC60000

    y15 y16 y17   0 y19 y20 y21 y22 y23 y24   0   0   0 y28 y29   0    0  0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  : (y << 15) & 0xEFC60000
    y00 y01 y02 y03 y04 y05 y06 y07 y08 y09 y10 y11 y12 y13 y14 y15 y16 y17 y18 y19 y20 y21 y22 y23 y24 y25 y26 y27 y28 y29 y30 y31  : y

     ^   ^   ^  y03  ^   ^   ^   ^   ^   ^  y10 y11 y12  ^   ^  y15 y16 y17 y18 y19 y20 y21 y22 y23 y24 y25 y26 y27 y28 y29 y30 y31  : res

     ^   ^   ^    0  ^   ^   ^   ^   ^   ^    0   0   0  ^   ^    0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  : res & 0xEFC60000
    y15 y16 y17 y18 y19 y20 y21 y22 y23 y24 y25 y26 y27 y28 y29 y30 y31   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  : res << 15

    y15 y16 y17   0 y19 y20 y21 y22 y23 y24   0   0   0 y28 y29  0    0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  : (res << 15) & 0xEFC60000
     ^   ^   ^  y03  ^   ^   ^   ^   ^   ^  y10 y11 y12  ^   ^  y15 y16 y17 y18 y19 y20 y21 y22 y23 y24 y25 y26 y27 y28 y29 y30 y31  : res

    y00 y01 y02 y03 y04

    So, the reverse is the same again (For this case) : res = res ^ ((res << 15) & 0xEFC60000)





    y = y ^ ((y << 7) & 0x9D2C5680UL)

    y00 y01 y02 y03 y04 y05 y06 y07 y08 y09 y10 y11 y12 y13 y14 y15 y16 y17 y18 y19 y20 y21 y22 y23 y24 y25 y26 y27 y28 y29 y30 y31  : y

    y07 y08 y09 y10 y11 y12 y13 y14 y15 y16 y17 y18 y19 y20 y21 y22 y23 y24 y25 y26 y27 y28 y29 y30 y31   0   0   0   0   0   0   0  : y << 7
      1   0   0   1   1   1   0   1   0   0   1   0   1   1   0   0   0   1   0   1   0   1   1   0   1   0   0   0   0   0   0   0  : 0x9D2C5680

    y07   0   0 y10 y11 y12   0 y14   0   0 y17   0 y19 y20   0   0   0 y24   0 y26   0 y28 y29   0 y31   0   0   0   0   0   0   0  : (y << 7) & 0x9D2C5680
    y00 y01 y02 y03 y04 y05 y06 y07 y08 y09 y10 y11 y12 y13 y14 y15 y16 y17 y18 y19 y20 y21 y22 y23 y24 y25 y26 y27 y28 y29 y30 y31  : y

     ^   y01 y02  ^  ^   ^  y06  ^  y08 y09  ^  y11  ^   ^  y14 y15 y16  ^  y18  ^  y20  ^   ^  y23  ^  y25 y26 y27 y28 y29 y30 y31  : res
     ^  y08 y09  ^  y11  ^   ^  y14 y15 y16  ^  y18  ^  y20  ^   ^  y23  ^  y25 y26 y27 y28 y29 y30 y31   0   0   0   0   0   0   0  : res << 7
    y14 y15 y16  ^  y18  ^  y20  ^   ^  y23  ^  y25 y26 y27 y28 y29 y30 y31   0   0   0   0   0   0   0   0   0   0   0   0   0   0  : res << 14

      1   0   0   1   1   1   0   1   0   0   1   0   1   1   0   0   0   1   0   1   0   1   1   0   1   0   0   0   0   0   0   0  : 0x9D2C5680
     ^    0   0   ^ y11  ^    0  y14  0   0   ^  0    ^  y20  0   0   0  ^    0 y26   0 y28 y29   0 y31   0   0   0   0   0   0   0  : (res << 7) & 0x9D2C5680
    y15 y16 y17   0 y19 y20 y21 y22 y23 y24   0   0   0 y28 y29  0    0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  : (res << 7) & 0x9D2C5680
     ^   ^   ^  y03  ^   ^   ^   ^   ^   ^  y10 y11 y12  ^   ^  y15 y16 y17 y18 y19 y20 y21 y22 y23 y24 y25 y26 y27 y28 y29 y30 y31  : res




 */

static uint32_t untemper(uint32_t y)
{
    y ^= y >> L;
    y ^= ((y << T) & C);

    y  =
         (((((y << 7) ^ (y << 14)) & (1UL << 31)) |
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
         ((y << 7) & (1UL << 7))) & B) ^ y ;

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

    // Tests

    /* for (uint32_t shift = 0; shift < 31; shift++) { */
    /*     uint32_t z = arc4random(); */
    /*     uint32_t z2 = z ^ (z >> shift); */
    /*     uint32_t z_orig = z2 ^ (z2 >> shift) ^ (z2 >> (2*shift)); */
    /*     printf("z = %" PRIu32 "; z2 = %" PRIu32 "; z_orig = %" PRIu32 "\n", z, z2, z_orig); */
    /*     if (z == z_orig) { */
    /*         printf("%2d: good\n", shift); */
    /*     } else { */
    /*         printf("%2d: not good\n", shift); */
    /*     } */
    /* } */


/*
0 :   (z2 << 7) ^ (z2 << 14)
3 :   (z2 << 7) ^ (z2 << 14) ^ (z2 << 21) ^ (z2 << 28)
4 :   (z2 << 7)
5 :   (z2 << 7) ^ (z2 << 14) ^ (z2 << 21)
7:    (z2 << 7)
10:   (z2 << 7) ^ (z2 << 14) ^ (z2 << 21)
12:   (z2 << 7) ^ (z2 << 14)
13:   (z2 << 7)
17:   (z2 << 7) ^ (z2 << 14)
19:   (z2 << 7)
21:   (z2 << 7)
22:   (z2 << 7)
24:   (z2 << 7)
*/

    for (int i = 0 ; i < 20; i++) {
        uint32_t z = arc4random();
        uint32_t z2 = z ^ ((z << S) & B);
        uint32_t z_reconstructed =
            (((((z2 << 7) ^ (z2 << 14)) & (1UL << 31)) |
              (((z2 << 7) ^ (z2 << 14) ^ (z2 << 21) ^ (z2 << 28)) & (1UL << 28)) |
              ((z2 << 7) & (1UL << 27)) |
              (((z2 << 7) ^ (z2 << 14) ^ (z2 << 21)) & (1UL << 26)) |
              ((z2 << 7) & (1UL << 24)) |
              (((z2 << 7) ^ (z2 << 14) ^ (z2 << 21)) & (1UL << 21)) |
              (((z2 << 7) ^ (z2 << 14)) & (1UL << 19)) |
              ((z2 << 7) & (1UL << 18)) |
              (((z2 << 7) ^ (z2 << 14)) & (1UL << 14)) |
              ((z2 << 7) & (1UL << 12)) |
              ((z2 << 7) & (1UL << 10)) |
              ((z2 << 7) & (1UL << 9)) |
              ((z2 << 7) & (1UL << 7))) & B) ^ z2;

        if (z == z_reconstructed) {
            printf("%10" PRIu32 ": good\n", z);
        } else {
            printf("%10" PRIu32 ": not good\n", z);
        }
    }

    if (memcmp(old, new, sizeof(old)) == 0) {
        printf("Test passed: RNG state reconstructed\n");
        exit(EXIT_SUCCESS);
    } else {
        printf("Test failed\n");
        exit(EXIT_FAILURE);
    }

}
