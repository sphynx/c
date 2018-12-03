#pragma once

#include <inttypes.h>

#define MAX_LABELS 1000

struct label_entry {
    uint16_t address;
    uint8_t declared;
    char *name;
};

extern struct label_entry
label_table[MAX_LABELS];

struct label_entry
*add_label(char *label);

void
print_labels(void);

void
init_label_table(void);
