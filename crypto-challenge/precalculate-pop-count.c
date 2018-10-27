#include <assert.h>
#include <inttypes.h>
#include <stdio.h>

#include "hamming.h"

static size_t
pop_count_naive(uint8_t byte) {
    size_t res = 0;
    for (; byte; byte >>= 1)
        res += byte & 1;
    return res;
}

int main(void) {
    uint8_t i = 0;
    printf("{");
    for (;;) {
        printf("%zu,", pop_count_naive(i));
        if (++i == 0) {
            break;
        }
        if (i % 16 == 0) {
            printf("\n ");
        }
    }
    printf("}\n");

    char* str1 = "this is a test";
    char* str2 = "wokka wokka!!!";
    assert(hamming_distance(str1, str2) == 37);
}
