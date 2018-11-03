#pragma once

#include <openssl/evp.h>

/*
 * Initializes EVP context.
 */
EVP_CIPHER_CTX* evp_init(void);

/*
 * Decrypts AES-encoded data in ECB mode. Takes byte string in, its
 * length, the key (should be 128 bit). Writes output to *out. Its
 * size is returned. Returns the size of the output or -1 if anything
 * goes wrong. Use handle_errors() to print error message.
 */
int
evp_aes_128_ecb_decrypt(EVP_CIPHER_CTX* ctx,
                        unsigned char *in,
                        int in_len,
                        unsigned char *key,
                        unsigned char *out);

/*
 * Decrypts AES-encoded data in CBC mode using EVP interface. Takes
 * byte string in, its length, the key and IV (should be both 128
 * bit). Writes output to *out. Its size is returned. Returns the size
 * of the output or -1 if anything goes wrong. Use handle_errors() to
 * print error message.
 */
int
evp_aes_128_cbc_decrypt(EVP_CIPHER_CTX* ctx,
                        unsigned char *in,
                        int in_len,
                        unsigned char *key,
                        unsigned char *iv,
                        unsigned char *out);


/*
 * Decrypts AES-encoded data in CBC mode using low-level OpenSSL
 * interface. Takes byte string in, its length, the key and IV (should
 * be both 128 bit). Writes output to *out. Its size is returned.
 * Returns the size of the output or -1 if anything goes wrong. Use
 * handle_errors() to print error message.
 */
int
aes_128_cbc_decrypt(
                    unsigned char *in,
                    int in_len,
                    unsigned char *key,
                    unsigned char *iv,
                    unsigned char *out);


/*
 * Encrypts AES-encoded data in ECB mode. Takes byte string in, its
 * length, the key (should be 128 bit). Writes output to *out. Its
 * size is returned. Returns the size of the output or -1 if anything
 * goes wrong. Use handle_errors() to print error message.
 */
int
aes_128_ecb_encrypt(EVP_CIPHER_CTX* ctx,
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
