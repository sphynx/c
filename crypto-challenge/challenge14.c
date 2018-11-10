#include <assert.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "aes.h"
#include "base64.h"
#include "random.h"

#define MAX_BLOCK_SIZE 128
#define MAX_PREFIX_SIZE 256
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

static int
decrypt_next_char(size_t block_size, size_t prefix_len,
                  unsigned char* known_chars, size_t known_len)
{
    size_t blocks_needed = 1 + (known_len + prefix_len) / block_size;
    // size_t padding_needed = block_size - 1 - (known_len % block_size);
    size_t total_size = block_size * blocks_needed;
    size_t padding_needed = total_size - prefix_len - known_len - 1;
    int res = -1;

    unsigned char *in = calloc(block_size, blocks_needed);
    unsigned char *out = malloc(total_size + block_size + MAX_SECRET_SIZE);

    unsigned char *char_block = NULL;

    if (oracle(in, padding_needed, out) < 0)
        goto early_exit;

    // Save the last relevant block of output for later comparison.
    unsigned char *last_relevant_block = out + (blocks_needed - 1) * block_size;
    char_block = malloc(block_size);
    memcpy(char_block, last_relevant_block, block_size);

    // Copy known_chars to the input for checking.
    memcpy(in + padding_needed, known_chars, known_len);

    for (unsigned int i = 0; i < 256; i++) {
        in[total_size - prefix_len - 1] = (unsigned char) i;
        if (oracle(in, total_size - prefix_len, out) < 0)
            goto early_exit;

        if (memcmp(last_relevant_block, char_block, block_size) == 0) {
            res = i;
            break;
        }
    }

early_exit:
    free(in);
    free(out);
    free(char_block);

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
    if (block_size == -1) {
        printf("can't determine block_size\n");
        exit(1);
    }

    ssize_t total_len = detect_total_len((size_t) block_size);
    if (total_len == -1) {
        printf("can't determine total_len\n");
        exit(1);
    }

    ssize_t guessed_prefix_len = detect_prefix_len((size_t) block_size);
    if (guessed_prefix_len == -1) {
        printf("can't determine prefix_len\n");
        exit(1);
    }

    ssize_t secret_len = total_len - guessed_prefix_len;
    assert(secret_len >= 0);

    printf("block_size=%zd, total_len=%zd, prefix_len=%zd, "
           "guessed_prefix_len=%zd, secret_len=%zd\n",
           block_size, total_len, prefix_len,
           guessed_prefix_len, secret_len);

    // Decrypt the message byte-by-byte using oracle.
    unsigned char* secret = calloc(MAX_SECRET_SIZE, 1);
    for (size_t i = 0; i < (size_t) secret_len; i++) {
        int next_char = decrypt_next_char(block_size, guessed_prefix_len, secret, i);
        if (next_char < 0) {
            printf("can't decrypt next char\n");
            exit(1);
        }
        secret[i] = (unsigned char) next_char;
    }

    // Write the message.
    fwrite(secret, 1, secret_len, stdout);
    free(secret);
}
