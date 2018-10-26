#include <string.h>

#include "base64.h"
#include "hex.h"

int main(void) {
    char* str = "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d";
    size_t hex_len = strlen(str);
    uint8_t* bytes = read_hex(str, hex_len);
    char* actu = base64_encode(bytes, hex_len / 2);
    char* expe = "SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t";

    if (strcmp(actu, expe) != 0) {
        printf("[FAIL]\n"
               "expected: %s\n"
               "actual:   %s\n",
               expe, actu);
        return 1;
    } else {
        printf("[OK] Challenge1 passed\n");
    }
    return 0;
}
