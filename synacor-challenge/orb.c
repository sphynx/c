#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PLUS   -1
#define MINUS  -2
#define STAR   -3
#define NONE    0

#define TARGET_I  3
#define TARGET_J  2
#define TARGET   31

// Path template which is then permuted and interpreted to find
// whether it matches the target or not.

// Constraints: #E - #W = 3, #N - #S = 3, so that we end up in the
// final cell. Total number of moves = 8. (Minimal is 6, but there is
// no solution for that).

// 13
// static char path[] = "eeeeeewwwwnnn";

// 12
// static char path[] = "eeeeeewwwnnn";
// static char path[] = "eeeeewwnnnns";
// static char path[] = "eeeewnnnnnss";
// static char path[] = "eeennnnnnsss";

// 10
static char path[] = "eeeewnnnns";

// 8
// static char path[] = "eeennnns";

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

        if (i == 0 && j == 0)
            return -1;
        if (i == 3 && j == 3)
            return -1;

        path++;
    }


    if (i != TARGET_I || j != TARGET_J)
        return -1;

    // printf("%d\n", val);
    return val;
}

static void swap(char *x, char *y)
{
    char tmp = *x;
    *x = *y;
    *y = tmp;
}

static void permute(size_t n, char *x)
{
    if (n == 1) {
        if (interpret(path) == TARGET) {
            printf("%s\n", path);
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

int main(void)
{
    permute(strlen(path), path);
    return 0;
}
