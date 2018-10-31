#include <openssl/conf.h>
#include <openssl/err.h>

#include "aes.h"

EVP_CIPHER_CTX* evp_init(void) {
    return EVP_CIPHER_CTX_new();
}

int evp_decrypt(EVP_CIPHER_CTX* ctx,
                unsigned char *in, int in_len,
                unsigned char *key,
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

void evp_cleanup(EVP_CIPHER_CTX* ctx) {
    EVP_CIPHER_CTX_free(ctx);
}

void handle_evp_errors(void) {
    ERR_print_errors_fp(stderr);
}
