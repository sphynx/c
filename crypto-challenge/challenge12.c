#include <assert.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "aes.h"
#include "base64.h"
#include "random.h"

#define MAX_BLOCK_SIZE 128
#define MAX_SECRET_SIZE 512

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

static int
decrypt_next_char(size_t block_size, unsigned char* known_chars, size_t known_len)
{
    size_t blocks_needed = 1 + known_len / block_size;
    size_t padding_needed = block_size - 1 - (known_len % block_size);
    size_t total_size = block_size * blocks_needed;
    int res = -1;

    unsigned char *in = calloc(block_size, blocks_needed);
    unsigned char *out = malloc((blocks_needed + 1) * block_size + MAX_SECRET_SIZE);

    if (oracle(in, padding_needed, out) < 0) {
        free(in);
        free(out);
        return -1;
    }

    // Save the last relevant block of output for later comparison.
    unsigned char *last_relevant_block = out + (blocks_needed - 1) * block_size;
    unsigned char *char_block = malloc(block_size);
    memcpy(char_block, last_relevant_block, block_size);

    // Setup `blocks_needed` of input for checking.
    memcpy(in + padding_needed, known_chars, known_len);

    for (unsigned int i = 0; i < 256; i++) {
        in[total_size - 1] = (unsigned char) i;
        if (oracle(in, total_size, out) < 0)
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

    ssize_t s_block_size = detect_block_size();
    if (s_block_size < 0) {
        fprintf(stderr, "can't detect block_size\n");
        exit(1);
    }

    size_t block_size = (size_t) s_block_size;
    int is_ecb = detect_ecb(block_size);
    printf("detected block_size = %zu, is_ecb = %d\n", block_size, is_ecb);

    // Decrypt the message byte-by-byte using oracle.
    size_t i = 0;
    unsigned char* decrypted = calloc(MAX_SECRET_SIZE, 1);
    for (;;) {
        int next_char = decrypt_next_char(block_size, decrypted, i);
        if (next_char == -1) {
            break;
        } else {
            decrypted[i] = (unsigned char) next_char;
            i++;
        }
    }

    // Write the message.
    fwrite(decrypted, 1, i - 1, stdout);
    free(decrypted);

    return 0;
}
