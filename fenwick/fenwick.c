#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

#define LSB(x) (-(x) & (x))

int a[SIZE] = {0};

static int
sum_fenwick(int i)
{
    int s = 0;
    while (i != 0)
        s += a[i], i -= LSB(i);
    return s;
}

static void
add_fenwick(int i, int k)
{
    while (i < SIZE)
        a[i] += k, i += LSB(i);
}

int main(void)
{
    uint32_t rix, rop, rv;
    int x = 0;

    for (int i = 0; i < TESTS; i++) {
        rix = arc4random_uniform(SIZE - 1) + 1; // 1 to SIZE-1
        rop = arc4random_uniform(2);

        if (rop) {
            x += sum_fenwick(rix);
        } else {
            rv = arc4random_uniform(31) - 15;
            add_fenwick(rix, rv);
        }
    }
    printf("Done: %d\n", x);
}
