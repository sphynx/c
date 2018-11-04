#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "aes.h"

#define MAX_INPUT_SIZE 1024

void
init_with_random_bytes(unsigned char *data, size_t data_len)
{
    for (unsigned int i = 0; i < data_len; i++) {
        data[i] = (unsigned char) arc4random();
    }
}

void test_encrypt_decrypt(unsigned char *in, size_t in_len)
{
    unsigned char key[16];
    unsigned char iv[16];

    init_with_random_bytes(key, 16);
    init_with_random_bytes(iv, 16);

    unsigned char* encrypted = malloc(in_len + 16);
    int encrypted_len = cbc_encrypt(in, in_len, key, iv, encrypted);

    unsigned char* decrypted = malloc(encrypted_len + 16);
    int decrypted_len = cbc_decrypt(encrypted, encrypted_len, key, iv, decrypted);

    if (in_len != (size_t) decrypted_len) {
        fprintf(stderr, "[FAIL] Length mismatch (encrypted_len=%d). "
                "Expected: %zu, actual: %d", encrypted_len,
                in_len, decrypted_len);
    }

    if (memcmp(in, decrypted, in_len) != 0) {
        fprintf(stderr,
                "[FAIL] Data mismatch between original and encrypted/decrypted\n");
    }

    free(encrypted);
    free(decrypted);
}

int main(void)
{
    unsigned char *in = malloc(MAX_INPUT_SIZE);
    init_with_random_bytes(in, MAX_INPUT_SIZE);

    for (int len = 0; len <= MAX_INPUT_SIZE; len++) {
        test_encrypt_decrypt(in, (size_t) len);
    }

    printf("[OK] CBC encrypt/decrypt test passed (%d runs)\n",
           MAX_INPUT_SIZE);
}
