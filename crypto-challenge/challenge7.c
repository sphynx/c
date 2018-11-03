#include <string.h>

#include "base64.h"
#include "io.h"
#include "aes.h"

int main(void) {
    char* b64text = read_non_space(stdin);
    if (b64text == NULL) {
        printf("read_non_space failed\n");
        exit(1);
    }

    size_t cyphertext_len;
    uint8_t* cyphertext = base64_decode(b64text, &cyphertext_len);

    unsigned char* key = (unsigned char*) "YELLOW SUBMARINE"; // 128 bit

    unsigned char* decrypted = malloc(cyphertext_len + 16); // inl + block_size

    EVP_CIPHER_CTX* ctx = evp_init();
    int decrypted_len = evp_aes_128_ecb_decrypt(ctx, cyphertext, cyphertext_len, key, decrypted);
    if (decrypted_len == -1) {
        handle_evp_errors();
    }
    evp_cleanup(ctx);

    // NUL-terminate.
    decrypted[decrypted_len] = '\0';

    printf("Decrypted text is:\n");
    printf("%s\n", decrypted);

    return 0;
}
