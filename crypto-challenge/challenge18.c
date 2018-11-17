#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "aes.h"
#include "base64.h"

int main(void)
{
    char *str =
        "L77na/nrFsKvynd6HzOoG7GHTLXsTVu9qvY/2sy"
        "LXzhPweyyMTJULu/6/kXX0KSvoOLSFQ==";

    size_t decoded_len;
    uint8_t *decoded = base64_decode(str, &decoded_len);

    unsigned char key[16] = "YELLOW SUBMARINE";
    uint64_t nonce = 0;

    unsigned char *decrypted = malloc(decoded_len);

    if (!ctr_decrypt(decoded, decoded_len, key, nonce, decrypted)) {
        fprintf(stderr, "CTR decryption error\n");
        exit(1);
    }

    if (fwrite(decrypted, 1, decoded_len, stdout) != decoded_len) {
        perror("fwrite");
        exit(1);
    }

    return 0;
}
