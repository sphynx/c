#include <assert.h>
#include <inttypes.h>
#include <string.h>

#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/aes.h>

#include "aes.h"
#include "endian.h"
#include "xor.h"
#include "pkcs7-padding.h"

/****************************************************************************/

/*
 * Utilitary stuff for EVP interface.
 */

EVP_CIPHER_CTX*
evp_init(void)
{
    return EVP_CIPHER_CTX_new();
}

void
evp_cleanup(EVP_CIPHER_CTX* ctx)
{
    EVP_CIPHER_CTX_free(ctx);
}

void
handle_evp_errors(void)
{
    ERR_print_errors_fp(stderr);
}

/****************************************************************************/

/*
 * ECB mode (Electronic Code Book) implemented with EVP high-level
 * OpenSSL interface. It's the simplest mode: the message is divided
 * into blocks, padded and each block is encrypted separately.
 */

int
evp_ecb_encrypt(EVP_CIPHER_CTX* ctx, unsigned char *in,
                int in_len, unsigned char *key,
                unsigned char *out)
{

    int curr_out_len = 0;
    int total_len = 0;

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key, NULL)) {
        return -1;
    }

    if (1 != EVP_EncryptUpdate(ctx, out, &curr_out_len, in, in_len)) {
        return -1;
    }
    total_len = curr_out_len;

    if (1 != EVP_EncryptFinal_ex(ctx, out + curr_out_len, &curr_out_len)) {
        return -1;
    }
    total_len += curr_out_len;

    return total_len;
}

int
evp_ecb_decrypt(EVP_CIPHER_CTX *ctx, unsigned char *in,
                int in_len, unsigned char *key,
                unsigned char *out)
{

    int curr_out_len = 0;
    int total_len = 0;

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key, NULL)) {
        return -1;
    }

    if (1 != EVP_DecryptUpdate(ctx, out, &curr_out_len, in, in_len)) {
        return -1;
    }
    total_len = curr_out_len;

    if (1 != EVP_DecryptFinal_ex(ctx, out + curr_out_len, &curr_out_len)) {
        return -1;
    }
    total_len += curr_out_len;

    return total_len;
}

/****************************************************************************/

/*
 * CBC mode (Cipher Block Chaining). In CBC mode, each block of
 * plaintext is XORed with the previous ciphertext block before being
 * encrypted. This way, each ciphertext block depends on all plaintext
 * blocks processed up to that point.

 * I am implementing that without EVP functions, since it gets in a
 * way.
 */

int
cbc_encrypt(unsigned char *in, int in_len,
            unsigned char *key, unsigned char *iv,
            unsigned char *out)
{
    unsigned char *padded;
    ssize_t padded_len =
        add_padding((uint8_t*) in, in_len, AES_BLOCK_SIZE, &padded);
    if (padded_len == -1) {
        return -1;
    }

    assert(padded_len % AES_BLOCK_SIZE == 0);

    int no_of_blocks = padded_len / AES_BLOCK_SIZE;
    unsigned char* curr_out = out;
    unsigned char* curr_in = padded;
    unsigned char* prev_out = iv;
    unsigned char* xored;

    AES_KEY aes_key;
    AES_set_encrypt_key(key, 128, &aes_key);

    for (int i = 0; i < no_of_blocks; i++) {
        xored = xor(curr_in, prev_out, AES_BLOCK_SIZE);
        AES_encrypt(xored, curr_out, &aes_key);
        prev_out = curr_out;
        curr_in += AES_BLOCK_SIZE;
        curr_out += AES_BLOCK_SIZE;
        free(xored);
    }

    free(padded);
    return curr_out - out;
}

int
cbc_decrypt(unsigned char *in, int in_len,
            unsigned char *key, unsigned char *iv,
            unsigned char *out)
{
    // We expect whole number of blocks.
    assert(in_len % AES_BLOCK_SIZE == 0);

    int no_of_blocks = in_len / AES_BLOCK_SIZE;
    unsigned char* curr_out = out;
    unsigned char* curr_in = in;
    unsigned char* prev_in = iv;
    unsigned char* xored;
    size_t bytes_decrypted = 0;

    AES_KEY aes_key;
    AES_set_decrypt_key(key, 128, &aes_key);

    for (int i = 0; i < no_of_blocks; i++) {
        AES_decrypt(curr_in, curr_out, &aes_key);
        xored = xor(curr_out, prev_in, AES_BLOCK_SIZE);
        memcpy(curr_out, xored, AES_BLOCK_SIZE);
        prev_in = curr_in;
        curr_in += AES_BLOCK_SIZE;
        curr_out += AES_BLOCK_SIZE;
        bytes_decrypted += AES_BLOCK_SIZE;
        free(xored);
    }

    ssize_t pad_len = padding_len(out, bytes_decrypted);
    if (pad_len == -1) {
        return -1;
    }

    return bytes_decrypted - (size_t) pad_len;
}

int
ctr_encrypt(unsigned char *in,
            int in_len,
            unsigned char *key,
            uint64_t nonce,
            unsigned char *out)
{
    // Set up AES key.
    AES_KEY aes_key;
    AES_set_encrypt_key(key, 128, &aes_key);

    // Prepare starting in block. First goes nonce, little endian,
    // then goes the counter.
    unsigned char *ctr_in_block = malloc(16);
    if (ctr_in_block == NULL) {
        return 0;
    }

    uint8_t *nonce_bytes = bytes_le_order(nonce);
    memcpy(ctr_in_block, nonce_bytes, 8);

    uint64_t counter = 0;
    unsigned char *counter_bytes;

    unsigned char *ctr_out_block = malloc(16);
    if (ctr_out_block == NULL) {
        free(ctr_in_block);
        return 0;
    }

    for (int i = 0; i < in_len; i++) {
        if (i % 16 == 0) {
            // Encrypt a new counter block.
            counter_bytes = bytes_le_order(counter++);
            memcpy(ctr_in_block + 8, counter_bytes, 8);
            AES_encrypt(ctr_in_block, ctr_out_block, &aes_key);
            free(counter_bytes);
        }
        out[i] = in[i] ^ ctr_out_block[i % 16];
    }

    free(ctr_in_block);
    free(ctr_out_block);

    return 1;
}


int
ctr_decrypt(unsigned char *in,
            int in_len,
            unsigned char *key,
            uint64_t nonce,
            unsigned char *out)
{
    return ctr_encrypt(in, in_len, key, nonce, out);
}
