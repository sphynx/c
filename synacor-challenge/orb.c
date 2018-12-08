#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PLUS   -1
#define MINUS  -2
#define STAR   -3
#define NONE    0

#define TARGET_I  3
#define TARGET_J  3
#define TARGET   30

#define CHECK_START_SQUARE
#define CHECK_PREMATURE_TARGET_ENERGY
#define CHECK_PREMATURE_TARGET_SQUARE

// Path template which is then permuted and interpreted to find
// whether it matches the target or not.

// Constraints: #E - #W = 3, #N - #S = 3, so that we end up in the
// final cell.

// I came up manually with 14-long path, but it was too long, so we
// should be able to put the path we are looking for into 12
// characters + '\0' terminator.
static char path[13];

// Map.
static char m[4][4] = { {22, MINUS, 9, STAR},
                        {PLUS, 4, MINUS, 18},
                        {4, STAR, 11, STAR},
                        {STAR, 8, MINUS, 1} };


static void step(int *op, int *val, int i, int j)
{
    if (*op < 0) {
        switch (*op) {
        case PLUS:  *val += m[i][j]; break;
        case STAR:  *val *= m[i][j]; break;
        case MINUS: *val -= m[i][j]; break;
        default:
            printf("unexpected op\n");
            exit(EXIT_FAILURE);
            break;
        }
        *op = NONE;
    } else
        *op = m[i][j];
}

static int interpret(char *path)
{
    int i = 0;
    int j = 0;
    int op = NONE;
    int val = m[i][j];

    while (*path) {
        switch (*path) {
        case 'n':
            if (i == 3) return -1;
            step(&op, &val, i, j);
            i++;
            break;

        case 's':
            if (i == 0) return -1;
            step(&op, &val, i, j);
            i--;
            break;

        case 'w':
            if (j == 0) return -1;
            step(&op, &val, i, j);
            j--;
            break;

        case 'e':
            if (j == 3) return -1;
            step(&op, &val, i, j);
            j++;
            break;

        default:
            printf("unexpected path symbol: %c\n", *path);
            exit(EXIT_FAILURE);
        }

        path++;

        // Should never come back to the start.
        #ifdef CHECK_START_SQUARE
        if (i == 0 && j == 0)
            return -1;
        #endif

        // If it's not the last turn.
        if (*path) {
            // We should only be in the last cell during last call.
            #ifdef CHECK_PREMATURE_TARGET_SQUARE
            if (i == 3 && j == 3)
                return -1;
            #endif

            // We shouldn't get the target energy prematurely.
            #ifdef CHECK_PREMATURE_TARGET_ENERGY
            if (val == TARGET)
                return -1;
            #endif
        }

        //printf("%d\n", val);
    }

    if (i != 3 || j != 3)
        return -1;

    return val;
}

static void swap(char *x, char *y)
{
    char tmp = *x;
    *x = *y;
    *y = tmp;
}

static void pretty(char *path)
{
    while (*path) {
        switch (*path) {
        case 'n': printf("north"); break;
        case 's': printf("south"); break;
        case 'e': printf("east");  break;
        case 'w': printf("west");  break;
        default:
            printf("unexpected path\n");
            exit(EXIT_FAILURE);
            break;
        }
        printf("\n");
        path++;
    }
}

static void permute(size_t n, char *x)
{
    if (n == 1) {
        //printf("%s\n", path);
        if (interpret(path) == TARGET) {
            pretty(path);
            exit(EXIT_SUCCESS);
        }

        return;
    }

    for (size_t i = 0; i < n; i++) {
        swap(x, x + i);
        permute(n - 1, x + 1);
        swap(x, x + i);
    }
}

static void try_paths(int len)
{
    int n, e, w, s;
    int i;

    printf("Trying paths of length %d\n", len);
    for (w = 0; w < len / 2; w++) {
        for (s = 0; s < len / 2; s++) {
            e = w + 3;
            n = s + 3;

            char *curr = path;
            if (n + e + w + s == len) {
                for (i = 0; i < e; i++) *curr++ = 'e';
                for (i = 0; i < w; i++) *curr++ = 'w';
                for (i = 0; i < n; i++) *curr++ = 'n';
                for (i = 0; i < s; i++) *curr++ = 's';

                *curr = '\0';

                printf("Permuting %s\n", path);
                permute(len, path);
            }
        }
    }
}

int main(void)
{
    try_paths(6);
    try_paths(8);
    try_paths(10);
    try_paths(12);
    return 0;
}
