#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aes.h"
#include "random.h"

static unsigned char key[16];
static unsigned char iv[16];

static int
oracle(const char *in, size_t in_len, unsigned char **out)
{
    assert(*out && in && in_len);

    const char *prefix = "comment1=cooking%20MCs;userdata=";
    const char *suffix = ";comment2=%20like%20a%20pound%20of%20bacon";

    const size_t prefix_len = strlen(prefix);
    const size_t suffix_len = strlen(suffix);
    const size_t total_len = prefix_len + in_len + suffix_len;

    unsigned char *buf = malloc(total_len);
    if (buf == NULL) {
        perror("malloc");
        exit(1);
    }

    // Set up encoding buffer.
    memcpy(buf, (unsigned char*) prefix, prefix_len);
    memcpy(buf + prefix_len, (unsigned char*) in, in_len);
    memcpy(buf + prefix_len + in_len, (unsigned char*) suffix, suffix_len);

    *out = malloc(total_len + 16);

    int encr_len = cbc_encrypt(buf, (int) total_len, key, iv, *out);

    free(buf);

    return encr_len;
}

static int
check_admin(unsigned char *in, size_t in_len)
{
    unsigned char *out = malloc(in_len + 16);
    int decr_len = cbc_decrypt(in, in_len, key, iv, out);
    if (decr_len == -1) {
        printf("decryption failed\n");
        exit(1);
    }

    out[decr_len] = '\0'; // NUL-terminate to use `strstr`.

    const char *needle = ";admin=true;";
    return (strstr((char *) out, needle) != NULL) ? 1 : 0;
}

static void
flip_msb(unsigned char *data) // flips most significant bit
{
    *data = *data ^ 0x80;
}

static size_t
get_padding_size(unsigned char *in, size_t in_len)
{
    assert(in && in_len);

    const size_t block_size = 16;
    size_t res = block_size;
    unsigned char *out = malloc(in_len + block_size);
    size_t num_blocks = in_len / block_size +
        (in_len % block_size == 0 ? 0 : 1);

    assert(num_blocks > 1);
    unsigned char *second_last_block = in + (num_blocks - 2) * block_size;

    for (unsigned int i = 1; i < block_size; i++) {
        flip_msb(&second_last_block[block_size - i - 1]);
        int decr = cbc_decrypt(in, in_len, key, iv, out);
        flip_msb(&second_last_block[block_size - i - 1]);
        if (decr > -1) {
            res = i;
            break;
        }
    }

    free(out);
    return res;
}

static void
tamper_ciphertext(unsigned char *in, size_t in_len)
{
    assert(in_len > 16);

    size_t num_blocks = in_len / 16 + (in_len % 16 == 0 ? 0 : 1);
    unsigned char *second_last_block = in + (num_blocks - 2) * 16;

    unsigned char mask[16];
    unsigned char want[16] = ";admin=true;\x04\x04\x04\x04";

    for (unsigned int i = 0; i < 16; i++) {
        mask[i] = want[i] ^ 16;
        second_last_block[i] ^= mask[i];
    }
}

int
main(void)
{
    unsigned char *encrypted;

    init_with_random_bytes(key, 16);
    memset(iv, 0, 16);

    const char *test_str = "x";
    int encr_len = oracle(test_str, strlen(test_str), &encrypted);
    if (encr_len == -1) {
        printf("encryption failed\n");
        exit(1);
    }

    if (check_admin(encrypted, encr_len)) {
        printf("admin access granted\n");
    } else {
        printf("no admin access\n");
    }

    size_t pad_size = get_padding_size(encrypted, encr_len);
    size_t total_data_size = encr_len - pad_size;
    size_t oracle_data_size = total_data_size - strlen(test_str);
    size_t required_data_size = encr_len - oracle_data_size;

    printf("padding size = %zu, total_data_size = %zu, "
           "oracle_data_size = %zu, required_data_size = %zu\n",
           pad_size, total_data_size, oracle_data_size, required_data_size);

    free(encrypted);

    char *buf = malloc(required_data_size);
    memset(buf, 'a', required_data_size);

    encr_len = oracle(buf, required_data_size, &encrypted);
    if (encr_len == -1) {
        printf("encryption failed\n");
        exit(1);
    }

    tamper_ciphertext(encrypted, encr_len);

    if (check_admin(encrypted, encr_len)) {
        printf("admin access granted\n");
    } else {
        printf("no admin access\n");
    }

    return 0;
}
