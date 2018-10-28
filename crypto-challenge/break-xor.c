#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base64.h"
#include "hamming.h"
#include "io.h"
#include "score.h"
#include "xor.h"

// #define DEBUG

static double
average(double* xs, size_t len) {
    size_t cnt = 0;
    double total = 0;
    for (size_t i = 0; i < len; i++) {
        cnt++;
        total += xs[i];
    }
    return total / cnt;
}

static void
print_key(uint8_t* key, size_t len) {
    printf("key=\"");
    for (size_t i = 0; i < len; i++) {
        printf("%c", key[i]);
    }
    printf("\"\n");
}

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

    int n = 15;
    for (key_size = 2; key_size <= 40; key_size++) {
        double dist[n];
        uint8_t* curr = cyphertext;
        for (int i = 0; i < n; i++) {
            dist[i] = ((double) hamming_distance_bytes(curr, curr + key_size, key_size)) / key_size;
            curr += 2 * key_size;
        }

        double norm_dist = average(dist, n);

        #ifdef DEBUG
        printf("key_size = %zu, norm_dist = %f\n", key_size, norm_dist);
        #endif

        if (norm_dist < best_dist) {
            best_dist = norm_dist;
            best_size = key_size;
        }
    }

    #ifdef DEBUG
    printf("best_size = %zu, best_dist = %f\n", best_size, best_dist);
    #endif

    uint8_t* key = malloc(best_size);
    for (size_t i = 0; i < best_size; i++) {
        uint8_t k = best_key_step(cyphertext, byte_len, i, best_size);
        key[i] = k;
    }

    print_key(key, best_size);

    printf("\nDecoded message:\n");
    uint8_t* decoded = xor_with_repeating_key(cyphertext, byte_len, key, best_size);
    if (fwrite(decoded, 1, byte_len, stdout) != byte_len) {
        printf("fwrite failed\n");
        exit(1);
    }

    free(key);
    free(decoded);
    free(b64text);
    free(cyphertext);
    fclose(stream);

    return 0;
}
