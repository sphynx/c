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


int main(void)
{
    int i = 0;
    char line[30];
    struct claim claims[CLAIMS];
    struct claim *c = claims;
    while (fgets(line, 30, stdin)) {
        if (i == CLAIMS) break;
        c = &claims[i++];
        sscanf(line, "#%d @ %d,%d: %dx%d",
               &c->id, &c->x, &c->y, &c->w, &c->h);
    }

    int max_x = 0;
    int max_y = 0;
    for (i = 0; i < CLAIMS; i++) {
        int right = claims[i].w + claims[i].x;
        int down = claims[i].h + claims[i].y;
        if (right > max_x)
            max_x = right;
        if (down > max_y)
            max_y = down;
    }

    printf("max_x = %d, max_y = %d\n", max_x, max_y);

    int field[max_x][max_y];
    memset(field, 0, sizeof(int) * max_x * max_y);

    for (i = 0; i < CLAIMS; i++)
        for (int x = claims[i].x; x < claims[i].x + claims[i].w; x++)
            for (int y = claims[i].y; y < claims[i].y + claims[i].h; y++)
                field[x][y]++;

    int counter = 0;
    for (i = 0; i < max_x; i++)
        for (int j = 0; j < max_y; j++)
            if (field[i][j] > 1) counter++;

    printf("answer = %d\n", counter);

    return 0;
}
