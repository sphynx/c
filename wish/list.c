#include <stdlib.h>

#include "list.h"

size_t
length(struct node *curr)
{
    size_t s = 0;
    while (curr != NULL) {
        curr = curr->next;
        s++;
    }
    return s;
}

void
add_arg(struct node **head, char* arg)
{
    struct node *new = malloc(sizeof(struct node));
    new->arg = arg;
    new->next = NULL;

    if (*head == NULL) {
        *head = new;
    } else {
        struct node *curr = *head;
        while (curr->next)
            curr = curr->next;

        curr->next = new;
    }
}

void
free_list(struct node *head)
{
    if (head == NULL)
        return;

    free_list(head->next);
    free(head);
}

