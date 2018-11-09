#include <assert.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "aes.h"
#include "base64.h"
#include "random.h"

#define MAX_BLOCK_SIZE 128
#define MAX_PREFIX_SIZE 64
#define MAX_SECRET_SIZE 512

static unsigned char key[16];
static size_t prefix_len;
static unsigned char *prefix;

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

    // Prepend randomly initialized `prefix_len` bytes and append
    // `secret` to the end of input.

    size_t new_in_len = prefix_len + in_len + secret_len;
    unsigned char *new_in = malloc(new_in_len);
    if (new_in == NULL) {
        return -1;
    }

    memcpy(new_in, prefix, prefix_len);
    memcpy(new_in + prefix_len, in, in_len);
    memcpy(new_in + prefix_len + in_len, secret, secret_len);

    // Encrypt with ECB.
    EVP_CIPHER_CTX* ctx = evp_init();
    int encrypted_len = evp_ecb_encrypt(ctx, new_in, new_in_len, key, out);
    evp_cleanup(ctx);

    free(new_in);

    return encrypted_len;
}

static ssize_t
detect_block_size(void)
{
    size_t first_increase = 0;
    size_t second_increase = 0;

    size_t prev_out_size;
    size_t curr_out_size;

    unsigned char *in;
    unsigned char *out;

    int res = -1;

    in = calloc(MAX_BLOCK_SIZE, 1);
    out = calloc(2 * MAX_BLOCK_SIZE + MAX_SECRET_SIZE, 1);
    if (in == NULL || out == NULL) {
        goto error;
    }

    prev_out_size = oracle(in, 1, out);
    if (prev_out_size < 0) {
        goto error;
    }

    for (size_t i = 2; i <= MAX_BLOCK_SIZE; i++) {
        curr_out_size = oracle(in, i, out);
        if (curr_out_size < 0) {
            goto error;
        } else {
            if ((size_t) curr_out_size != prev_out_size) {
                prev_out_size = curr_out_size;
                if (first_increase == 0) {
                    first_increase = i;
                } else {
                    second_increase = i;
                    break;
                }
            }
        }
    }

    res = first_increase == 0 || second_increase == 0
        ? -1 : second_increase - first_increase;

error:
    free(in);
    free(out);

    return res;
}

static int
detect_ecb(size_t block_size)
{
    unsigned char *two_blocks = calloc(2 * block_size, 1);
    unsigned char *encrypted = malloc(3 * block_size + MAX_SECRET_SIZE);

    int res = -1;

    if (oracle(two_blocks, 2 * block_size, encrypted) < 0)
        goto error;

    res = (0 == memcmp(encrypted, encrypted + block_size, block_size));

error:
    free(two_blocks);
    free(encrypted);

    return res;
}


static ssize_t
detect_total_len(size_t block_size)
{
    ssize_t res = -1;
    unsigned char *out = malloc(MAX_SECRET_SIZE + 2 * block_size + MAX_PREFIX_SIZE);

    unsigned char *in = malloc(block_size);
    int enc_size = oracle(in, 0, out);
    if (enc_size == -1)
        goto early_exit;

    for (size_t i = 1; i <= block_size; i++) {
        if (oracle(in, i, out) != enc_size) {
            res = (enc_size / block_size - 1) * block_size +
                (block_size - i);
            break;
        }
    }

early_exit:
    free(out);
    return res;
}


static ssize_t
detect_prefix_len(ssize_t block_size)
{
    ssize_t res = -1;

    size_t out_size = MAX_SECRET_SIZE + 2 * block_size + MAX_PREFIX_SIZE;

    unsigned char *out1 = malloc(out_size);
    unsigned char *out2 = malloc(out_size);

    unsigned char *one = calloc(block_size, 1);
    one[block_size - 1] = 0x01;

    unsigned char *two = calloc(block_size, 1);
    two[block_size - 1] = 0x02;

    int enc_size = oracle(&one[block_size - 1], 1, out1);
    if (enc_size == -1)
        goto early_exit;

    oracle(&two[block_size - 1], 1, out2);

    size_t lower_limit;
    for (size_t i = block_size; i <= (size_t) enc_size; i += block_size) {
        if (memcmp(out1, out2, i) == 0) {
            lower_limit = i;
        } else {
            break;
        }
    }

    size_t upper_limit = lower_limit + block_size - 1;
    for (size_t i = upper_limit; i >= lower_limit; i--) {
        size_t pad_size = block_size - (i % block_size);

        enc_size = oracle(&one[block_size - pad_size - 1],
                          pad_size + 1, out1);
        oracle(&two[block_size - pad_size - 1],
               pad_size + 1, out2);

        if (memcmp(out1, out2, i) == 0) {
            res = i;
            break;
        }
    }

early_exit:
    free(out1);
    free(out2);
    free(one);
    free(two);
    return res;
}

int main(void)
{
    init_with_random_bytes(key, 16);

    // Initialize random prefix.
    prefix_len = (size_t) arc4random_uniform(MAX_PREFIX_SIZE + 1);
    prefix = malloc(prefix_len);

    init_with_random_bytes(prefix, prefix_len);

    ssize_t block_size = detect_block_size();
    int is_ecb = detect_ecb(block_size);
    ssize_t total_len = detect_total_len((size_t) block_size);
    ssize_t guessed_prefix_len = detect_prefix_len((size_t) block_size);
    ssize_t secret_len = total_len - guessed_prefix_len;

    printf("block_size=%zd, is_ecb=%d, total_len=%zd, prefix_len=%zd, "
           "guessed_prefix_len=%zd, secret_len=%zd\n",
           block_size, is_ecb, total_len, prefix_len,
           guessed_prefix_len, secret_len);
}
