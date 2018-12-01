#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#define INPUT_LEN 1016
#define SET_SIZE  400000
#define OFFSET    (SET_SIZE / 2)

static int     input[INPUT_LEN];
static uint8_t seen[SET_SIZE] = {0};

int main(void)
{
    int sum = 0;
    int curr;
    int i = 0;

    while (scanf("%d", &curr) > 0)
        input[i++] = curr;

    for (;;)
        for (i = 0; i < INPUT_LEN; i++) {
            sum += input[i];
            int loc = sum + OFFSET;
            if (loc >= 0 && loc < SET_SIZE) {
                if (seen[loc]) {
                    printf("%d\n", sum);
                    exit(EXIT_SUCCESS);
                } else
                    seen[loc] = 1;
            } else {
                printf("set overflow: loc=%d\n", loc);
                exit(EXIT_FAILURE);
            }
        }

    return EXIT_FAILURE;
}
