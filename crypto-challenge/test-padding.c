#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pkcs7-padding.h"

#define TEST_SIZE 512

void test_add_and_remove_padding(uint8_t *in, size_t in_len, size_t block_size)
{
    uint8_t *padded;

    ssize_t new_len = add_padding(in, in_len, block_size, &padded);
    if (new_len == -1) {
        fprintf(stderr, "add_padding failed\n");
    }

    ssize_t pad_len = padding_len(padded, (size_t) new_len);
    if (pad_len == -1) {
        fprintf(stderr, "padding_len failed\n");
    }

    if (in_len != new_len - (size_t) pad_len) {
        fprintf(stderr, "[FAIL] Lengths mismatch (block_size=%zu). "
                "Expected: %zu, actual: %zu\n", block_size,
                in_len, new_len - (size_t) pad_len);
    }

    if (memcmp(in, padded, in_len) != 0) {
        fprintf(stderr, "[FAIL] Data mismatch between padding input and output\n");
    }

    free(padded);
}

void test_on_random_bytes()
{
    // Initialize random byte string.
    uint8_t *in = malloc(TEST_SIZE);
    for (int i = 0; i < TEST_SIZE; i++) {
        in[i] = (uint8_t) arc4random();
    }

    for (int len = 0; len <= TEST_SIZE; len++) {
        test_add_and_remove_padding(in, len, 2);
        test_add_and_remove_padding(in, len, 4);
        test_add_and_remove_padding(in, len, 8);
        test_add_and_remove_padding(in, len, 16);
        test_add_and_remove_padding(in, len, 255);
    }

    printf("[OK] Padding test passed\n");
}

int main(void)
{
    test_on_random_bytes();
}
