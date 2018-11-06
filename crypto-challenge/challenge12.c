#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "aes.h"
#include "base64.h"
#include "random.h"

enum mode { CBC, ECB };

static unsigned char key[16];

static int
oracle(unsigned char *in, size_t in_len, unsigned char *out)
{
    size_t secret_len;
    char *secret_str =
        "Um9sbGluJyBpbiBteSA1LjAKV2l0aCBteSByYWctdG9wIGRvd24gc28gbXkg"
        "aGFpciBjYW4gYmxvdwpUaGUgZ2lybGllcyBvbiBzdGFuZGJ5IHdhdmluZyBq"
        "dXN0IHRvIHNheSBoaQpEaWQgeW91IHN0b3A/IE5vLCBJIGp1c3QgZHJvdmUg"
        "YnkK";

    uint8_t* secret = base64_decode(secret_str, &secret_len);
    if (secret == NULL) {
        fprintf(stderr, "base64_decode failed\n");
        exit(1);
    }

    // Append `secret` to the end of input:
    size_t new_in_len = in_len + secret_len;
    unsigned char *new_in = malloc(new_in_len);
    if (new_in == NULL) {
        return -1;
    }
    memcpy(new_in, in, in_len);
    memcpy(new_in + in_len, secret, secret_len);

    // Encrypt with ECB.
    EVP_CIPHER_CTX* ctx = evp_init();
    int encrypted_len = evp_ecb_encrypt(ctx, new_in, new_in_len, key, out);
    evp_cleanup(ctx);

    free(new_in);

    return encrypted_len;
}


int main(void)
{
    init_with_random_bytes(key, 16);

    return 0;
}
