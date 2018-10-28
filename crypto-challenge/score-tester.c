/*
 * Test scoring by first reading from the file and decoding N chars
 * blocks then 2*N, 4*N, 8*N, 16*N, etc. until all the text fits into
 * one block, then stop. This way, we can test that scoring function
 * is able to handle well different lenghts of messages.
 */

#include <string.h>
#include <stdlib.h>

#include "score.h"
#include "xor.h"


#define TOO_SHORT 15

// FIXME: we should actually turn those into program options.
#define KEY 'X'
#define INITIAL_BUF_SIZE 20
// Comment if you want to test key retrieval instead of strings
// comparison.
#define CMP_DECODED_STRINGS

struct stats {
    size_t buf_size;
    int total;
    int ok;
    int fail;
};


static void
print_stats(struct stats* stats) {
    if (stats->buf_size == 0) {
        printf("overall:\n");
    } else {
        printf("buf_size = %zu:\n", stats->buf_size);
    }
    printf("total %6d\n"
           "ok    %6d\n"
           "fail  %6d\n"
           "rate %6.2f%%\n",
           stats->total, stats->ok, stats->fail,
           (double) stats->ok / (double) stats->total * 100.0);
    printf("\n");
}

#ifdef CMP_DECODED_STRINGS
static void
print_mismatch_bytes(uint8_t* orig, char* decoded, size_t len) {
    printf("[mismatch]:\noriginal: ");
    for (size_t i = 0; i < len; i++) {
        printf("%c", orig[i]);
    }
    printf("\ndecoded : ");
    for (size_t i = 0; i < len; i++) {
        printf("%c", decoded[i]);
    }
    printf("\n");
}
#else
static void
print_mismatch_keys(uint8_t orig, uint8_t decoded) {
    printf("[mismatch] original key: %c, decoded key: %c\n",
           orig, decoded);
}
#endif

int
main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: score-tester <file>");
        exit(1);
    }

    FILE* stream = fopen(argv[1], "rb");

    size_t read_bytes;
    uint8_t* xored;

    struct stats glob_stats = {.ok = 0, .fail = 0, .total = 0, .buf_size = 0};
    size_t buf_size = INITIAL_BUF_SIZE;
    uint8_t* buf = malloc(buf_size);

    if (buf == NULL) {
        perror("malloc");
        exit(1);
    }

    for (;;) {
        struct stats curr_stats =
            {.ok = 0, .fail = 0, .total = 0, .buf_size = buf_size};

        for (;;) {
            read_bytes = fread(buf, 1, buf_size, stream);
            if (read_bytes < TOO_SHORT) {
                break;
            } else {
                xored = xor_with_byte_key(buf, KEY, read_bytes);
                #ifdef CMP_DECODED_STRINGS
                char* decoded;
                best_score_raw(xored, read_bytes, &decoded);
                int success = (memcmp(decoded, buf, read_bytes) == 0);
                #else
                size_t start = 4;
                size_t step = 5;
                uint8_t k = best_key_step(xored, read_bytes, start, step);
                int success = (k == KEY);
                #endif

                if (success) {
                    curr_stats.ok++;
                    glob_stats.ok++;
                } else {
                    #ifdef CMP_DECODED_STRINGS
                    print_mismatch_bytes(buf, decoded, read_bytes);
                    #else
                    print_mismatch_keys(KEY, k);
                    #endif

                    curr_stats.fail++;
                    glob_stats.fail++;
                }
                free(xored);

                #ifdef CMP_DECODED_STRINGS
                free(decoded);
                #endif

                curr_stats.total++;
                glob_stats.total++;
            }
        }

        print_stats(&curr_stats);

        // Check the current position in the file
        long pos = ftell(stream);
        if (pos == -1) {
            perror("ftell");
        } else if (pos < 0) {
            printf("unexpected, pos is < 0\n");
            exit(1);
        }

        unsigned long upos = (unsigned long) pos;
        if (upos < buf_size) {
            // Either we read it all in one go and there is no point
            // of increasing the buffer again or there was an error
            // and we exited too early.
            break;
        } else {
            // Reset the file, increase the buffer size, reallocate
            // the buffer and run the tests again.
            if (fseek(stream, 0L, SEEK_SET) == -1) {
                perror("fseek");
                exit(1);
            }
            buf_size *= 2;
            buf = realloc(buf, buf_size);
            if (buf == NULL) {
                perror("realloc");
                exit(1);
            }
        }
    }
    free(buf);
    fclose(stream);

    print_stats(&glob_stats);

    return 0;
}
