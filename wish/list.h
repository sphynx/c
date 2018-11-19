#pragma once

struct node {
    char *val;
    struct node *next;
};

size_t
length(struct node *curr);

void
add_elem(struct node **head, char* elem);

void
free_list(struct node *curr);
