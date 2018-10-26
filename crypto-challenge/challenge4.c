#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freq.h"

int main(void) {
    char* line = NULL;
    size_t capacity;

    char* curr_best_line;
    char* overall_best_line;
    int best_so_far = INT_MAX;

    FILE* f = fopen("data/4.txt", "rb");

    while (getline(&line, &capacity, f) != -1) {
        *strrchr(line, '\n') = '\0';

        int curr_score = best_score_hex(line, &curr_best_line);
        if (curr_score < best_so_far) {
            best_so_far = curr_score;
            overall_best_line = strdup(curr_best_line);
        }
        free(curr_best_line);
    }
    printf("final score: %d, final line: %s\n", best_so_far, overall_best_line);
    return 0;
}
