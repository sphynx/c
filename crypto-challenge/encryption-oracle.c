#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "aes.h"
#include "random.h"

static int
oracle(unsigned char *in, size_t in_len, unsigned char *out)
{
    unsigned char key[16];
    int encrypted_len;

    init_with_random_bytes(key, 16);

    // Append 5-10 bytes at the beginning and the end.
    uint32_t begin_bytes = arc4random_uniform(6) + 5;
    uint32_t end_bytes = arc4random_uniform(6) + 5;

    // Allocate a new buffer for input and those extra bytes.
    unsigned char *new_in = malloc(in_len + begin_bytes + end_bytes);
    if (new_in == NULL) {
        return -1;
    }

    // Initialise extra bytes randomly.
    arc4random_buf(new_in, begin_bytes);
    memcpy(new_in + begin_bytes, in, in_len);
    arc4random_buf(new_in + begin_bytes + in_len, end_bytes);

    // Randomly encrypt with either ECB or CBC (uniformly).
    if (arc4random_uniform(2) == 0) {
        unsigned char iv[16];
        init_with_random_bytes(iv, 16);
        encrypted_len = cbc_encrypt(in, in_len, key, iv, out);
    } else {
        EVP_CIPHER_CTX* ctx = evp_init();
        encrypted_len = evp_ecb_encrypt(ctx, in, in_len, key, out);
        evp_cleanup(ctx);
    }

    free(new_in);

    return encrypted_len;
}

int main(void)
{
    char *test = "HELLO";
    size_t len = strlen(test) - 1;
    unsigned char *str = (unsigned char *) test;
    unsigned char *encrypted = malloc(len + 16);
    int encrypted_len = oracle(str, len, encrypted);
    fwrite(encrypted, 1, encrypted_len, stdout);

    return 0;
}
