#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

int main(void)
{
    struct timeval t[(int)1e6];
    size_t total = sizeof(t) / sizeof(struct timeval);

    for (unsigned int i = 0; i < total; i++) {
        gettimeofday(&t[i], NULL);
    }

    printf("%zu calls: from %d to %d\n",
           total, t[0].tv_usec, t[total-1].tv_usec);
    double pace = (double) (t[total-1].tv_usec - t[0].tv_usec) / total;
    printf("%.1f ns per call\n", pace * 1000);

    return 0;
}
