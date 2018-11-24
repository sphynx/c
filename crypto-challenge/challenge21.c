#include <stdio.h>

#include "mersenne_twister.h"

int main(void)
{
    seed(0);

    // Super basic statistical test, checks whether the lowest
    // significant bit is random enough :)
    int even = 0;
    int odd = 0;
    for (int i = 0; i < 10000; i++) {
        uint32_t r = extract();
        if (r % 2 == 0)
            even++;
        else
            odd++;
    }

    printf("even: %d, odd: %d\n", even, odd);
    return 0;
}
