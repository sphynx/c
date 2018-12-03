#include <stdio.h>
#include <string.h>

#define CLAIMS 1323

struct claim {
    int id;
    int x;
    int y;
    int w;
    int h;
};

static int
overlaps(struct claim *c1, struct claim *c2)
{
    struct claim *left;
    struct claim *right;
    struct claim *top;
    struct claim *bottom;

    if (c1->x < c2->x) {
        left = c1;
        right = c2;
    } else {
        left = c2;
        right = c1;
    }

    if (c1->y < c2->y) {
        top = c1;
        bottom = c2;
    } else {
        top = c2;
        bottom = c1;
    }

    return
        (left->x + left->w > right->x) &&
        (top->y  + top->h  > bottom->y);
}

int main(void)
{
    int i = 0;
    int j = 0;
    char line[30];
    struct claim claims[CLAIMS];
    struct claim *c = claims;

    while (fgets(line, 30, stdin)) {
        if (i == CLAIMS) break;
        c = &claims[i++];
        sscanf(line, "#%d @ %d,%d: %dx%d",
               &c->id, &c->x, &c->y, &c->w, &c->h);
    }

    for (i = 0; i < CLAIMS; i++) {
        for (j = 0; j < CLAIMS; j++) {
            if (i == j) continue;
            if (overlaps(&claims[i], &claims[j])) break;
        }
        if (j == CLAIMS) {
            printf("id: %d\n", claims[i].id);
            break;
        }
    }

    return 0;
}
