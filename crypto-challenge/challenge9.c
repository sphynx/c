#include <stdio.h>
#include <string.h>

#include "pkcs7-padding.h"

int main(void)
{
    uint8_t* str = (uint8_t*) "YELLOW SUBMARINE";
    uint8_t* res;
    ssize_t new_len = add_padding(str, strlen((char*) str), 20, &res);
    if (new_len == -1) {
        fprintf(stderr, "add_padding failed\n");
        exit(1);
    }
    fwrite(res, 1, new_len, stdout);
    free(res);
}
