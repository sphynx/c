#include <assert.h>
#include <string.h>
#include <openssl/conf.h>
#include <openssl/err.h>

#include "aes.h"
#include "xor.h"

EVP_CIPHER_CTX*
evp_init(void) {
    return EVP_CIPHER_CTX_new();
}

int
aes_128_ecb_decrypt(EVP_CIPHER_CTX *ctx, unsigned char *in,
                    int in_len, unsigned char *key,
                    unsigned char *out) {

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


int
aes_128_cbc_decrypt(EVP_CIPHER_CTX *ctx, unsigned char *in,
                    int in_len, unsigned char *key, unsigned char *iv,
                    unsigned char *out) {

    int curr_out_len = 0;
    int total_len = 0;

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key, NULL)) {
        return -1;
    }

    int block_size = EVP_CIPHER_CTX_block_size(ctx);
    int no_of_blocks = in_len / block_size;

    unsigned char* curr = out;
    unsigned char* chain = iv;

    assert(in_len % block_size == 0);

    EVP_CIPHER_CTX_set_padding(ctx, 0);

    for (int i = 0; i < no_of_blocks; i++) {
        if (1 != EVP_DecryptUpdate(ctx, curr, &curr_out_len,
                                   in + i * block_size,
                                   block_size)) {
            return -1;
        }

        assert(curr_out_len == block_size);

        unsigned char* xored = xor(curr, chain, curr_out_len);
        memcpy(curr, xored, (size_t) curr_out_len);
        free(xored);

        chain = in + i * block_size;
        curr += curr_out_len;
        total_len += curr_out_len;
    }

    if (1 != EVP_DecryptFinal_ex(ctx, out + total_len, &curr_out_len)) {
        return -1;
    }

    assert(curr_out_len == 0);

    // total_len += curr_out_len;

    return total_len;
}



int
aes_128_ecb_encrypt(EVP_CIPHER_CTX* ctx, unsigned char *in,
                    int in_len, unsigned char *key,
                    unsigned char *out) {

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


void evp_cleanup(EVP_CIPHER_CTX* ctx) {
    EVP_CIPHER_CTX_free(ctx);
}

void handle_evp_errors(void) {
    ERR_print_errors_fp(stderr);
}
