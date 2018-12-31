#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "random.h"
#include "mersenne_twister.h"

void
mt_encrypt(unsigned char *in,
           int in_len,
           uint16_t key,
           unsigned char *out)
{
    unsigned char keystream[4];
    seed((uint32_t) key);

    // Now we extract, 4 bytes at a time and use it as a key stream to
    // xor with plaintext.

    for (int i = 0; i < in_len; i++) {
        if (i % 4 == 0) {
            // Prepare the next 4 bytes of the keystream.
            uint32_t k = extract();
            keystream[0] = (unsigned char) ( k & 0x000000FF       );
            keystream[1] = (unsigned char) ((k & 0x0000FFFF) >>  8);
            keystream[2] = (unsigned char) ((k & 0x00FFFFFF) >> 16);
            keystream[3] = (unsigned char) (k                >> 24);
        }
        out[i] = in[i] ^ keystream[i % 4];
    }
}

void
mt_decrypt(unsigned char *in,
           int in_len,
           uint16_t key,
           unsigned char *out)
{
    mt_encrypt(in, in_len, key, out);
}

int main(void)
{
    char *test = "a long test string, longer than 4 characters!";

    size_t len = strlen(test) + 1;
    unsigned char *encr = malloc(len);
    unsigned char *decr = malloc(len);

    if (encr == NULL || decr == NULL) {
        printf("out of memory\n");
        exit(1);
    }

    uint16_t key = (uint16_t) arc4random();

    mt_encrypt((unsigned char *) test, len, key, encr);
    mt_decrypt(encr, len, key, decr);

    printf("after turnaround: %s\n", decr);

    free(encr);
    free(decr);

    return 0;
}
