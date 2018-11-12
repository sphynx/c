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

int
main(void)
{
    unsigned char *encrypted;

    init_with_random_bytes(key, 16);
    memset(iv, 0, 16);

    const char *test_str = "test";
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

    if (fwrite(encrypted, 1, (size_t) encr_len, stdout) != (size_t) encr_len) {
        perror("fwrite failed");
        exit(1);
    }

    return 0;
}
