#include <assert.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "aes.h"
#include "base64.h"
#include "pkcs7-padding.h"
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
        "MDAwMDAzQ29va2luZyBNQydzIGxpa2UgYSBwb3VuZCBvZiBiYWNvbg==",
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
check_padding(unsigned char *in, size_t in_len)
{
    unsigned char *out = malloc(in_len + 16);
    unsigned char *iv = calloc(16, 1);
    int rc = cbc_decrypt(in, in_len, key, iv, out);
    free(out);
    free(iv);
    return rc >= 0;
}

static void
decrypt_block(const unsigned char *ct_orig, unsigned char *pt, size_t real_pad)
{
    unsigned char *ct = malloc(32);
    memcpy(ct, ct_orig, 32);

    // First write real padding (if present, i.e. real_pad > 0) to CT
    // and prepare PT.
    for (int byte_ix = 15; byte_ix + real_pad > 15; byte_ix--) {
        pt[byte_ix] = (unsigned char) real_pad;
        ct[byte_ix] ^= (real_pad ^ (real_pad + 1));
    }

    // Now continue decyphering byte by byte (in reversed order) using
    // padding oracle attack. Try to find a byte `patch` which doesn't
    // give a padding error.
    for (int byte_ix = 15 - real_pad; byte_ix >= 0; byte_ix--) {
        for (int patch = 0; patch < 256; patch++) {
            ct[byte_ix] ^= patch;
            if (check_padding(ct, 32)) {
                uint8_t pad = 16 - byte_ix;
                pt[byte_ix] = patch ^ pad;

                // Increase padding of tail bytes to prepare for the
                // next padding probe.
                for (int k = byte_ix; k <= 15; k++)
                    ct[k] ^= (pad ^ (pad + 1));

                break;
            }

            // Padding error: undo the change
            ct[byte_ix] ^= patch;
            if (patch == 255) {
                printf("could not decrypt byte :(\n");
            }
        }
    }

    free(ct);
}

static size_t
detect_padding_size(const unsigned char *ct_orig)
{
    unsigned char *ct = malloc(32);
    memcpy(ct, ct_orig, 32);

    size_t padding = 0;
    for (int byte_ix = 15; byte_ix >= 0; byte_ix--) {
        ct[byte_ix] ^= 1UL;
        int ok = check_padding(ct, 32);
        ct[byte_ix] ^= 1UL;

        if (!ok)
            padding++;
        else
            break;
    }

    free(ct);

    return padding;
}

static void
decrypt(unsigned char *in, size_t in_len, unsigned char *iv, unsigned char *out)
{
    assert(in_len % 16 == 0 && in_len > 0);

    unsigned char *curr_ct, *curr_pt;
    size_t num_blocks = in_len / 16;

    // Decrypt the first block (we have to put together IV and first
    // block of CT).
    unsigned char *iv_ct = malloc(32);
    memcpy(iv_ct, iv, 16);
    memcpy(iv_ct + 16, in, 16);
    curr_pt = out;
    decrypt_block(iv_ct, curr_pt, 0);
    free(iv_ct);

    // Decrypt middle blocks.
    curr_ct = in;
    curr_pt = out + 16;
    for (unsigned int i = 0; i < num_blocks - 2; i++) {
        decrypt_block(curr_ct, curr_pt, 0);
        curr_ct += 16;
        curr_pt += 16;
    }

    // Decrypt the last block with real padding size.
    size_t padding_size = detect_padding_size(curr_ct);
    decrypt_block(curr_ct, curr_pt, padding_size);
}

int main(void)
{
    init_with_random_bytes(key, 16);

    unsigned char *encrypted = malloc(1024); // should be enough for our purposes ;)
    unsigned char *iv = malloc(16);
    int encr_len = oracle(encrypted, iv);

    // Decrypt using oracle padding attack.
    unsigned char *decrypted = calloc(1024, 1);
    decrypt(encrypted, encr_len, iv, decrypted);

    // Drop the real padding from the result.
    ssize_t decr_padding = padding_len(decrypted, encr_len);
    if (decr_padding == -1) {
        printf("final padding error\n");
        exit(1);
    }

    fwrite(decrypted, 1, encr_len - decr_padding, stdout);

    return 0;
}
