#include <stdio.h>
#include <string.h>

#include "pkcs7-padding.h"

int main(void) {
    uint8_t* str = (uint8_t*) "YELLOW SUBMARINE";
    uint8_t* res;
    ssize_t new_len = pkcs7_pad(str, strlen((char*) str), 20, &res);
    if (new_len == -1) {
        fprintf(stderr, "pkcs7_padstr failed\n");
        exit(1);
    }
    fwrite(res, 1, new_len, stdout);
    free(res);
}
