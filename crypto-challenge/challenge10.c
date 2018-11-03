#include "base64.h"
#include "io.h"
#include "aes.h"

int main(void) {
    size_t cyphertext_len;
    uint8_t* cyphertext = base64_decode(read_non_space(stdin), &cyphertext_len);

    unsigned char* key = (unsigned char*) "YELLOW SUBMARINE";
    unsigned char iv[16] = {0};

    unsigned char* decrypted = malloc(cyphertext_len + 16); // inl + block_size
    int decrypted_len = cbc_decrypt(cyphertext, cyphertext_len, key, iv, decrypted);

    fwrite(decrypted, 1, decrypted_len, stdout);

    return 0;
}
