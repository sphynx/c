#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "aes.h"
#include "random.h"

enum mode { CBC, ECB };

#define BLOCK_SIZE 16

static int
oracle(unsigned char *in, size_t in_len, unsigned char *out, enum mode *used_mode)
{
    unsigned char key[16];
    int encrypted_len;

    init_with_random_bytes(key, 16);

    // Append 5-10 bytes at the beginning and the end.
    uint32_t begin_bytes = arc4random_uniform(6) + 5;
    uint32_t end_bytes = arc4random_uniform(6) + 5;

    // Allocate a new buffer for input and those extra bytes.
    size_t new_in_len = in_len + begin_bytes + end_bytes;
    unsigned char *new_in = malloc(new_in_len);
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
        encrypted_len = cbc_encrypt(new_in, new_in_len, key, iv, out);
        *used_mode = CBC;
    } else {
        EVP_CIPHER_CTX* ctx = evp_init();
        encrypted_len = evp_ecb_encrypt(ctx, new_in, new_in_len, key, out);
        evp_cleanup(ctx);
        *used_mode = ECB;
    }

    free(new_in);

    return encrypted_len;
}

/*
 * Send 43 identical bytes (0x00 in this case), that is enough to
 * handle random addition of 5-10 bytes at the beginning and the end
 * and check if the second and ther third returned blocks are
 * identical. They should be if it's ECB and they are most likely not
 * if it's CBC.
 */
void guess(void)
{
    unsigned char zeros[43] = {0};
    unsigned char *encrypted = malloc(43 + BLOCK_SIZE);
    enum mode used_mode;
    enum mode guessed_mode;

    (void) oracle(zeros, 43, encrypted, &used_mode);

    if (memcmp(encrypted + BLOCK_SIZE,
               encrypted + 2 * BLOCK_SIZE,
               BLOCK_SIZE) == 0) {
        guessed_mode = ECB;
    } else {
        guessed_mode = CBC;
    }

    printf("used mode:    %s\n", used_mode == ECB ? "ECB" : "CBC");
    printf("guessed mode: %s\n", guessed_mode == ECB ? "ECB" : "CBC");

    if (used_mode == guessed_mode) {
        printf("guessed well!\n");
    } else {
        printf("meh :(\n");
    }

    free(encrypted);
}


int main(void)
{
    for (int i = 0; i < 10; i++) {
        guess();
    }
    return 0;
}
