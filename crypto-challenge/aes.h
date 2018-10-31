#pragma once

#include <openssl/evp.h>

/*
 * Initializes EVP context.
 */
EVP_CIPHER_CTX* evp_init(void);

/*
 * Decrypts AES-encoded data. Takes byte string in, its length, the
 * key (should be 128 bit). Writes output to *out. Its size is
 * returned. Returns the size of the output or -1 if anything goes
 * wrong. Use handle_errors() to print error message.
 */
int evp_decrypt(EVP_CIPHER_CTX* ctx,
                unsigned char *in,
                int in_len,
                unsigned char *key,
                unsigned char *out);

/*
 * Cleans up EVP context.
 */
void evp_cleanup(EVP_CIPHER_CTX* ctx);

/*
 * Function to call if we want to inspect EVP errors.
 */
void handle_evp_errors(void);
