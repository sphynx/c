#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "usage: %s <size_in_mb> <time_in_sec>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    errno = 0;
    long size_in_meg = strtol(argv[1], NULL, 10);
    long time_to_run = strtol(argv[2], NULL, 10);
    if (errno != 0) {
        perror("strtol");
        exit(EXIT_FAILURE);
    }

    pid_t pid = getpid();
    printf("pid: %ld\n", (long) pid);

    struct timeval start;
    struct timeval curr;
    long size_to_alloc = size_in_meg * 1024 * 1024;

    printf("allocating %ld bytes...\n", size_to_alloc);
    unsigned char *arr = malloc(size_to_alloc);
    if (arr == NULL) {
        printf("Not enough memory\n");
        exit(EXIT_FAILURE);
    }

    memset(arr, 0, size_to_alloc);
    gettimeofday(&start, NULL);

    long times = 0;
    for (;;) {
        for (long i = 0; i < size_to_alloc; i++)
            arr[i]++;

        times++;

        int resolution = 1;
        if (size_in_meg >= 1 && size_in_meg < 500) {
            resolution = 500 / size_in_meg;
        }

        if (times % resolution == 0) {
            gettimeofday(&curr, NULL);
            double diff =
                (curr.tv_sec - start.tv_sec) * 1e6 +
                (curr.tv_usec - start.tv_usec);
            printf("elapsed: %.2fs\n", diff / 1e6);
            if (diff > time_to_run * 1e6) {
                printf("enough\n");
                exit(EXIT_SUCCESS);
            }
        }
    }

    return 0;
}
