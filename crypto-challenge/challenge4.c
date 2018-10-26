#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "score.h"

int main(void) {
    char* line = NULL;
    size_t capacity = 0;

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

    char* actu = overall_best_line;
    char* expe = "Now that the party is jumping\n";

    if (strcmp(actu, expe) != 0) {
        printf("[FAIL]\n"
               "expected: %s\n"
               "actual:   %s\n",
               expe, actu);
        return 1;
    } else {
        printf("[OK] Challenge4 passed\n");
    }
    return 0;
}
