#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"


int a[SIZE] = {0};

static int
sum_naive(int i)
{
    int s = 0;
    for (int j = 1; j <= i; j++)
        s += a[j];
    return s;
}

static void
add_naive(int i, int k)
{
    a[i] += k;
}

int main(void)
{
    uint32_t rix, rop, rv;
    int x = 0;

    for (int i = 0; i < TESTS; i++) {
        rix = arc4random_uniform(SIZE - 1) + 1; // 1 to SIZE-1
        rop = arc4random_uniform(2);

        if (rop) {
            x += sum_naive(rix);
        } else {
            rv = arc4random_uniform(31) - 15;
            add_naive(rix, rv);
        }
    }
    printf("Done: %d\n", x);
}
