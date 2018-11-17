#pragma once

#include <openssl/evp.h>

/*
 * Initializes EVP context.
 */
EVP_CIPHER_CTX
*evp_init(void);

/*
 * Cleans up EVP context.
 */
void
evp_cleanup(EVP_CIPHER_CTX* ctx);

/*
 * Function to call if we want to inspect EVP errors.
 */
void
handle_evp_errors(void);

/*
 * Encrypts AES-encoded data in ECB mode. Takes byte string in, its
 * length, the key (should be 128 bit). Writes output to out. Its size
 * is returned. Returns the size of the output or -1 if anything goes
 * wrong. Use handle_errors() to print error message.
 */
int
evp_ecb_encrypt(EVP_CIPHER_CTX* ctx,
                unsigned char *in,
                int in_len,
                unsigned char *key,
                unsigned char *out);

/*
 * Decrypts AES-encoded data in ECB mode using EVP interface. Takes
 * byte string in, its length, the key (should be 128 bit). Writes
 * output to out. Its size is returned. Returns the size of the
 * output or -1 if anything goes wrong. Use handle_errors() to print
 * error message.
 */
int
evp_ecb_decrypt(EVP_CIPHER_CTX* ctx,
                unsigned char *in,
                int in_len,
                unsigned char *key,
                unsigned char *out);


/*
 * Encrypts AES-encoded data in CBC mode using low-level OpenSSL
 * interface. Takes byte string in, its length, the key and IV (should
 * be both 128 bit). Writes output to out. Its size is returned.
 * Returns the size of the output or -1 if anything goes wrong.
 * Handles padding.
 */
int
cbc_encrypt(unsigned char *in,
            int in_len,
            unsigned char *key,
            unsigned char *iv,
            unsigned char *out);

/*
 * Decrypts AES-encoded data in CBC mode using low-level OpenSSL
 * interface. Takes byte string in, its length, the key and IV (should
 * be both 128 bit). Writes output to out. Its size is returned.
 * Returns the size of the output or -1 if anything goes wrong.
 * Handles padding.
 */
int
cbc_decrypt(unsigned char *in,
            int in_len,
            unsigned char *key,
            unsigned char *iv,
            unsigned char *out);


/*
 * Encrypts/decrypts AES-encoded data in CTR mode using low-level
 * OpenSSL interface. Takes a byte string in, its length, the key and
 * the nonce. Writes to out (its length should be the same as the
 * length of in). Returns 0 if failed, 1 if everything is good.
 * Encryption and decryption are identical. No padding is made because
 * it's not needed.
 */
int
ctr_encrypt(unsigned char *in,
            int in_len,
            unsigned char *key,
            uint64_t nonce,
            unsigned char *out);

int
ctr_decrypt(unsigned char *in,
            int in_len,
            unsigned char *key,
            uint64_t nonce,
            unsigned char *out);
