#include <string.h>

#include "freq.h"
#include "xor.h"

#define KEY 'X'

int main(void) {
    char* line = NULL;
    size_t capacity = 0;
    ssize_t len;

    int ok = 0;
    int fail = 0;
    int total = 0;

    uint8_t* xored;
    char* decoded;

    while ((len = getline(&line, &capacity, stdin)) > 0) {
        if (len < 15) continue; // skip short lines
        xored = xor_with_byte((uint8_t*) line, KEY, len);
        best_score_raw(xored, len, &decoded);
        if (strcmp(decoded, line) == 0) {
            ok++;
        } else {
            printf("[mismatch]:\noriginal: %s\ndecoded : %s\n", line, decoded);
            fail++;
        }
        total++;
    }

    printf("scoring: total[%3d], ok[%3d], fail[%3d], rate=%6.2f%%\n",
           total, ok, fail, (double) ok / (double) total * 100.0);

    return 0;
}
