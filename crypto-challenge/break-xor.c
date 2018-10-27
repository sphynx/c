#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base64.h"
#include "hamming.h"
#include "io.h"
#include "score.h"
#include "xor.h"

int main(void) {
    FILE* stream = fopen("data/6.txt", "rb");
    if (stream == NULL) {
        perror("fopen");
        exit(1);
    }

    char* b64text = read_non_space(stream);
    if (b64text == NULL) {
        printf("read_non_space failed\n");
        exit(1);
    }

    size_t byte_len;
    uint8_t* cyphertext = base64_decode(b64text, &byte_len);
    if (cyphertext == NULL) {
        printf("base64_decode failed\n");
        exit(1);
    }

    size_t key_size;
    double best_dist = 1e9;
    size_t best_size = 0;

    // TODO: implement a more robust approach, maybe sort them and
    // take 3 best.
    for (key_size = 2; key_size <= 40; key_size++) {
        size_t dist1 = hamming_distance_bytes(cyphertext,
                                             cyphertext + key_size, key_size);

        size_t dist2 = hamming_distance_bytes(cyphertext + 2 * key_size,
                                             cyphertext  + 3 * key_size, key_size);

        double norm_dist1 = (double) dist1 / key_size;
        double norm_dist2 = (double) dist2 / key_size;

        double norm_dist = (norm_dist1 + norm_dist2) / 2;

        printf("key_size = %zu, norm_dist = %f\n", key_size, norm_dist);
        if (norm_dist < best_dist) {
            best_dist = norm_dist;
            best_size = key_size;
        }
    }

    printf("best_size = %zu, best_dist = %f\n", best_size, best_dist);

    uint8_t* key = malloc(best_size);
    for (size_t i = 0; i < best_size; i++) {
        uint8_t k = best_key_step(cyphertext, byte_len, i, best_size);
        printf("key[%zu] = %" PRIu8 "\n", i, k);
        key[i] = k;
    }

    uint8_t* decoded = xor_with_repeating_key(cyphertext, byte_len, key, best_size);
    if (fwrite(decoded, 1, byte_len, stdout) != byte_len) {
        printf("fwrite failed\n");
        exit(1);
    }

    free(b64text);
    free(cyphertext);
    free(key);
    free(decoded);
    fclose(stream);

    return 0;
}
