#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int *x = malloc(1000);
    *x = 12;
    printf("%d\n", *x);
    return 0;
}
