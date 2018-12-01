#include <stdio.h>

int main(void)
{
    int sum = 0;
    int lines = 0;
    int curr;

    while (scanf("%d", &curr) > 0) {
        sum += curr;
        lines++;
    }

    printf("lines read: %d\n", lines);
    printf("answer: %d\n", sum);
    return 0;
}
