#include <stdio.h>
#include <stdlib.h>

static int one_char_diff(char *x, char *y)
{
    int total = 0;
    for (int i = 0; i < 26; i++)
        if (x[i] != y[i]) {
            if (total == 0)
                total = 1;
            else
                return 0;
        }

    return total == 1 ? 1 : 0;
}

int main(void)
{
    char lines[251][30];

    int i = 0;
    while (fgets(lines[i++], 30, stdin))
        ;

    for (int j = 0; j < 250; j++)
        for (int k = j + 1; k < 250; k++)
            if (one_char_diff(lines[j], lines[k])) {
                // Print common chars.
                for (int l = 0; l < 26; l++)
                    if (lines[j][l] == lines[k][l])
                        putchar(lines[j][l]);

                printf("\n");
                exit(0);
            }

    return 1;
}
