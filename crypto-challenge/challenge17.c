#include <assert.h>
#include <stdlib.h>

#include "aes.h"
#include "base64.h"
#include "random.h"

static unsigned char key[16];

static int
oracle(unsigned char *out, unsigned char *iv)
{
    assert(out && iv);

    char *inputs[] = {
        "MDAwMDAwTm93IHRoYXQgdGhlIHBhcnR5IGlzIGp1bXBpbmc=",
        "MDAwMDAxV2l0aCB0aGUgYmFzcyBraWNrZWQgaW4gYW5kIHRoZSBWZWdhJ3MgYXJlIHB1bXBpbic=",
        "MDAwMDAyUXVpY2sgdG8gdGhlIHBvaW50LCB0byB0aGUgcG9pbnQsIG5vIGZha2luZw==",
        "MDAwMDAzQ29va2luZyBNQydzIGxpa2UgYSBwb3VuZCBvZiBiYWNvbg=="
        "MDAwMDA0QnVybmluZyAnZW0sIGlmIHlvdSBhaW4ndCBxdWljayBhbmQgbmltYmxl",
        "MDAwMDA1SSBnbyBjcmF6eSB3aGVuIEkgaGVhciBhIGN5bWJhbA==",
        "MDAwMDA2QW5kIGEgaGlnaCBoYXQgd2l0aCBhIHNvdXBlZCB1cCB0ZW1wbw==",
        "MDAwMDA3SSdtIG9uIGEgcm9sbCwgaXQncyB0aW1lIHRvIGdvIHNvbG8=",
        "MDAwMDA4b2xsaW4nIGluIG15IGZpdmUgcG9pbnQgb2g=",
        "MDAwMDA5aXRoIG15IHJhZy10b3AgZG93biBzbyBteSBoYWlyIGNhbiBibG93"
    };

    size_t pool_size = sizeof(inputs) / sizeof(inputs[0]);

    uint8_t random_ix = (uint8_t) arc4random_uniform(pool_size);

    size_t decoded_len;
    uint8_t* decoded = base64_decode(inputs[random_ix], &decoded_len);

    init_with_random_bytes(iv, 16);

    return cbc_encrypt(decoded, decoded_len, key, iv, out);
}

static int
check_padding(unsigned char *in, size_t in_len, unsigned char *iv)
{
    unsigned char *out = malloc(in_len + 16);
    int rc = cbc_decrypt(in, in_len, key, iv, out);
    free(out);
    return rc >= 0;
}

int main(void)
{
    init_with_random_bytes(key, 16);

    unsigned char *out = malloc(1024); // should be enough for our purposes ;)
    unsigned char *iv = malloc(16);
    int encr_len = oracle(out, iv);
    out[encr_len - 1] = 17;
    int padding_ok = check_padding(out, encr_len, iv);
    printf("check_padding = %d\n", padding_ok);
    return 0;
}
