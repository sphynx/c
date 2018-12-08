#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#define SIZE 32768
static uint16_t cache[SIZE][SIZE];
static uint16_t x;

uint16_t pseudo_ackermann(uint16_t m, uint16_t n)
{
    uint16_t res = cache[m][n];
    if (res)
        return res;

    if (m == 0)
        res = (n + 1) % SIZE;
    else if (n == 0)
        res = pseudo_ackermann(m - 1, x);
    else
        res = pseudo_ackermann(m - 1, pseudo_ackermann(m, n - 1));

    cache[m][n] = res;
    return res;
}

int main(void)
{
    x = 0;
    for (;;) {
        memset(cache, 0, sizeof(cache));
        uint16_t res = pseudo_ackermann(4, 1);
        printf("%05" PRIu16 ": %d\n", x, res);
        if (res == 6) {
            printf("Found answer: %" PRIu16 "\n", x);
            return 0;
        }
        if (++x == 0) break;
    }
    return 1;
}
