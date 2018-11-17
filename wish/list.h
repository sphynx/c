#pragma once

struct node {
    char *arg;
    struct node *next;
};

size_t
length(struct node *curr);

void
add_arg(struct node **head, char* arg);

void
free_list(struct node *curr);
