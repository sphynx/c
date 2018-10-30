#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hex.h"

int main(void) {

    char* line = NULL;
    size_t capacity = 0;
    ssize_t len;

    size_t line_no = 1; // we start from one here!

    while ((len = getline(&line, &capacity, stdin)) > 0) {
        line[len - 1] = '\0'; // truncate \n
        uint8_t* buf = read_hex(line, len - 1);
        //size_t buf_size = (len - 1) / 2;

        // now we have 10-blocks of 16-bytes (128 bits) and we have to
        // determine which of those could be produced by AES-128 ECB
        // (Electronic Code Book) mode.

        // We can just look if any pair of those blocks (there are
        // 10*9 / 2 = 45 of them) are the same.

        // FIXME: remove hardcoded numbers
        for (size_t i = 0; i < 10; i++) {
            for (size_t j = i + 1; j < 10; j++) {
                if (memcmp(buf + 16*i, buf + 16*j, 16) == 0) {
                    printf("found equal blocks, line: %zu, i: %zu, j: %zu\n",
                           line_no, i, j);
                }
            }
        }
        line_no++;

        free(buf);
    }
}
