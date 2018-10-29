/*
 * Based on OpenSSL wiki:
 * https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption
 */

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>

#include "base64.h"
#include "io.h"

void handleErrors(void) {
    ERR_print_errors_fp(stderr);
    abort();
}

int decrypt(unsigned char *ciphertext, int ciphertext_len,
            unsigned char *key, unsigned char *plaintext) {

    // Create and initialise the context.
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL) {
        handleErrors();
    }

    // Initialise the decryption operation.
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key, NULL)) {
        handleErrors();
    }

    // Provide the message to be decrypted, and obtain the plaintext
    // output. EVP_DecryptUpdate can be called multiple times if
    // necessary.
    int len;
    if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len)) {
        handleErrors();
    }
    int plaintext_len = len;

    // Finalise the decryption. Further plaintext bytes may be written
    // at this stage.
    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) {
        handleErrors();
    }
    plaintext_len += len;

    // Clean up.
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}

int main(void) {
    char* b64text = read_non_space(stdin);
    if (b64text == NULL) {
        printf("read_non_space failed\n");
        exit(1);
    }

    size_t cyphertext_len;
    uint8_t* cyphertext = base64_decode(b64text, &cyphertext_len);

    unsigned char* key = (unsigned char*) "YELLOW SUBMARINE"; // 128 bit

    unsigned char* decrypted = malloc(2 * cyphertext_len);
    int decrypted_len = decrypt(cyphertext, cyphertext_len, key, decrypted);

    // NUL-terminate.
    decrypted[decrypted_len] = '\0';

    printf("Decrypted text is:\n");
    printf("%s\n", decrypted);

    return 0;
}
