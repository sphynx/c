#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 151
static char m[SIZE][SIZE];

struct cart_node {
    int x;
    int y;
    char dir;
    struct cart_node *next;
};

static struct cart_node *carts = NULL;

static void add_cart(int x, int y, char c)
{
    struct cart_node *cart = malloc(sizeof(struct cart_node));
    cart->x = x;
    cart->y = y;
    cart->dir = c;
    cart->next = carts;
    carts = cart;
}

static int carts_len(void)
{
    struct cart_node *curr;
    int size = 0;
    for (curr = carts; curr != NULL; curr = curr->next)
        size++;
    return size;
}

static void read_map(void)
{
    char *line = NULL;
    size_t cap = 0;
    ssize_t line_len;
    int i = 0;

    while ((line_len = getline(&line, &cap, stdin)) != -1) {
        memcpy(m[i++], line, line_len - 1);
        if (i == SIZE) break;
    }

    free(line);
}

static void find_carts(char c)
{
    char *curr;
    for (int i = 0; i < SIZE; i++) {
        curr = m[i];
        for (;;) {
            curr = strchr(curr, c);
            if (curr == NULL) break;
            int j = (int) (curr - m[i]);
            printf("%c: %d %d\n", c, i, j);
            curr++;
            add_cart(j, i, c);
        }
    }
}

int main(void)
{
    read_map();
    find_carts('<');
    find_carts('>');
    find_carts('^');
    find_carts('v');

    printf("total: %d\n", carts_len());

    return 0;
}
