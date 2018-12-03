#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "table.h"

extern void yyerror(char *msg);

struct label_entry label_table[MAX_LABELS];

void init_label_table(void)
{
    memset(label_table, 0, sizeof(label_table));
}

struct label_entry *add_label(char *label)
{
    struct label_entry *e;
    for (e = label_table; e < &label_table[MAX_LABELS]; e++) {
        if (e->name != NULL && strcmp(e->name, label) == 0)
            return e;

        if (e->name == NULL) {
            e->name = strdup(label);
            e->declared = 0;
            return e;
        }
    }

    yyerror("too many labels");
    exit(1);
}

void print_labels(void) {
    struct label_entry *e;
    for (e = label_table; e < &label_table[MAX_LABELS]; e++) {
        if (e->name != NULL) {
            fprintf(stderr, "%s: %" PRIu16 "\n", e->name, e->address);
        } else {
            break;
        }
    }
}
