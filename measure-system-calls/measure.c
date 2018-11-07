#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>

// Returns delta in microseconds (1e-6 s).
static double
timeval_delta(struct timeval *start, struct timeval *finish)
{
    return (finish->tv_sec - start->tv_sec) * 1e6 +
        (finish->tv_usec - start->tv_usec);
}

// Returns delta in nanoseconds (1e-9 s).
static double
timespec_delta(struct timespec *start, struct timespec *finish)
{
    return (finish->tv_sec - start->tv_sec) * 1e9 +
        (finish->tv_nsec - start->tv_nsec);
}

// Returns time per call in nanoseconds.
static double
timeval_pace(struct timeval *start, struct timeval *finish, size_t total)
{
    return timeval_delta(start, finish) / total * 1000.0;
}

// Returns time per call in nanoseconds.
static double
timespec_pace(struct timespec *start, struct timespec *finish, size_t total)
{
    return timespec_delta(start, finish) / total;
}

static void
measure_gettimeofday(void)
{
    struct timeval start;
    struct timeval finish;
    size_t total = (size_t) 1e6;

    printf("Running %zu `gettimeofday` calls...\n", total);
    gettimeofday(&start, NULL);
    for (unsigned int i = 0; i < total - 1; i++) {
        gettimeofday(&finish, NULL);
    }

    double pace = timeval_pace(&start, &finish, total);
    printf("%.1f ns per `gettimeofday` call\n", pace);
}

static void
measure_clock_gettime_monotonic(void)
{
    struct timespec start;
    struct timespec finish;
    size_t total = (size_t) 1e6;

    printf("Running %zu `clock_gettime` calls...\n", total);
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (unsigned int i = 0; i < total - 1; i++) {
        clock_gettime(CLOCK_MONOTONIC, &finish);
    }

    double pace = timespec_pace(&start, &finish, total);
    printf("%.1f ns per `clock_gettime(CLOCK_MONOTONIC)` call\n", pace);
}

static void
measure_clock_gettime_monotonic_raw(void)
{
    struct timespec start;
    struct timespec finish;
    size_t total = (size_t) 1e6;

    printf("Running %zu `clock_gettime` calls...\n", total);
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for (unsigned int i = 0; i < total - 1; i++) {
        clock_gettime(CLOCK_MONOTONIC_RAW, &finish);
    }

    double pace = timespec_pace(&start, &finish, total);
    printf("%.1f ns per `clock_gettime(CLOCK_MONOTONIC_RAW)` call\n", pace);
}

static void
measure_read_with_gtod(void)
{
    size_t total = (size_t) 1e6;

    unsigned char *buf = malloc(1);
    if (buf == NULL) {
        exit(1);
    }

    struct timeval start;
    struct timeval stop;

    printf("Running %zu `read` calls...\n", total);
    gettimeofday(&start, NULL);
    for (unsigned int i = 0; i < total; i++) {
        read(STDIN_FILENO, buf, 0);
    }
    gettimeofday(&stop, NULL);

    double pace = timeval_pace(&start, &stop, total);
    printf("%.1f ns per `read` call (measured with `gettimeofday`)\n",
           pace);
}

static double
measure_read_with_cgt(void)
{
    size_t total = (size_t) 1e6;

    unsigned char *buf = malloc(1);
    if (buf == NULL) {
        exit(1);
    }

    struct timespec start;
    struct timespec stop;

    printf("Running %zu `read` calls...\n", total);
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for (unsigned int i = 0; i < total; i++) {
        read(STDIN_FILENO, buf, 0);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &stop);

    double pace = timespec_pace(&start, &stop, total);
    printf("%.1f ns per `read` call (measured with CLOCK_MONOTONIC_RAW)\n",
           pace);
    return pace;
}


static void
measure_write_with_gtod(void)
{
    size_t total = (size_t) 1e6;

    unsigned char *buf = malloc(1);
    if (buf == NULL) {
        exit(1);
    }

    struct timeval start;
    struct timeval stop;

    printf("Running %zu `write` calls...\n", total);
    gettimeofday(&start, NULL);
    for (unsigned int i = 0; i < total; i++) {
        write(STDOUT_FILENO, buf, 0);
    }
    gettimeofday(&stop, NULL);

    double pace = timeval_pace(&start, &stop, total);
    printf("%.1f ns per `write` call (measured with `gettimeofday`)\n",
           pace);
}

static double
measure_write_with_cgt(void)
{
    size_t total = (size_t) 1e6;

    unsigned char *buf = malloc(1);
    if (buf == NULL) {
        exit(1);
    }

    struct timespec start;
    struct timespec stop;

    printf("Running %zu `write` calls...\n", total);
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for (unsigned int i = 0; i < total; i++) {
        write(STDOUT_FILENO, buf, 0);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &stop);

    double pace = timespec_pace(&start, &stop, total);
    printf("%.1f ns per `write` call (measured with CLOCK_MONOTONIC_RAW)\n", pace);
    return pace;
}


static void
measure_context_switch(double avg_read_time, double avg_write_time)
{
    size_t total = (size_t) 5e5;

    // Create two pipes.
    int pipe1_fds[2];
    int pipe2_fds[2];

    if (pipe(pipe1_fds) == -1) {
        perror("pipe");
        exit(1);
    }
    if (pipe(pipe2_fds) == -1) {
        perror("pipe");
        exit(1);
    }

    printf("Running %zu context switches...\n", total * 2);

    int rc = fork();

    if (rc < 0) {
        perror("fork");
        exit(1);
    } else if (rc == 0) {
        // child
        unsigned char out_buf[] = {1};
        unsigned char in_buf[1];

        for (size_t i = 0; i < total; i++) {
            read(pipe2_fds[0], in_buf, 1);
            write(pipe1_fds[1], out_buf, 1);
        }
    } else {
        // parent
        unsigned char out_buf[] = {1};
        unsigned char in_buf[1];

        struct timespec start;
        struct timespec stop;
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);

        for (size_t i = 0; i < total; i++) {
            write(pipe2_fds[1], out_buf, 1);
            read(pipe1_fds[0], in_buf, 1);
        }

        clock_gettime(CLOCK_MONOTONIC_RAW, &stop);
        double pace = timespec_pace(&start, &stop, 2*total);
        printf("%.1f ns per context switch (including two sytem calls: read and write)\n", pace);
        printf("%.1f ns per context switch only\n",
               pace - avg_read_time - avg_write_time);

    }
}

int main(void)
{
    measure_gettimeofday();
    measure_clock_gettime_monotonic();
    measure_clock_gettime_monotonic_raw();
    measure_read_with_gtod();
    double read_time = measure_read_with_cgt();
    measure_write_with_gtod();
    double write_time = measure_write_with_cgt();
    measure_context_switch(read_time, write_time);
    return 0;
}
