#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

static uint16_t buf[MEMORY_SIZE];

static void
data_str(const uint16_t data, char *str)
{
    if (is_register(data)) {
        snprintf(str, 8, "%%%d", reg_no(data));
    } else {
        snprintf(str, 8, "%" PRIu16, data);
    }
}

static void
raw_str(const uint16_t data, char *str)
{
    snprintf(str, 8, "%" PRIu16, data);
}

static void
char_str(const uint16_t data, char *str)
{
    if (is_register(data)) {
        snprintf(str, 8, "%%%d", reg_no(data));
    } else {
        if (data == 10) {
            snprintf(str, 8, "'\\n'");
        } else {
            snprintf(str, 8, "'%c'", (char) data);
        };
    }
}

int
main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s <vm_program>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE *in = fopen(argv[1], "rb");
    if (in == NULL) {
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }


    size_t read_bytes = fread(buf, 2, MEMORY_SIZE, in);
    if (read_bytes == 0) {
        if (feof(in)) {
            err("empty program provided");
            exit(EXIT_FAILURE);
        }

        if (ferror(in)) {
            perror("fread");
            exit(EXIT_FAILURE);
        }
    }

    uint16_t ip = 0;
    char a[8];
    char b[8];
    char c[8];

    for (;;) {
        if (ip >= read_bytes) {
            break;
        }

        printf("%05" PRIu16 ":  ", ip);

        switch (buf[ip++]) {
        case 0:
            printf("halt\n");
            break;

        case 1:
            data_str(buf[ip++], a);
            data_str(buf[ip++], b);
            printf("set %s %s\n", a, b);
            break;

        case 2:
            data_str(buf[ip++], a);
            printf("push %s\n", a);
            break;

        case 3:
            data_str(buf[ip++], a);
            printf("pop %s\n", a);
            break;

        case 4:
            data_str(buf[ip++], a);
            data_str(buf[ip++], b);
            data_str(buf[ip++], c);
            printf("eq %s %s %s\n", a, b, c);
            break;

        case 5:
            data_str(buf[ip++], a);
            data_str(buf[ip++], b);
            data_str(buf[ip++], c);
            printf("gt %s %s %s\n", a, b, c);
            break;

        case 6:
            data_str(buf[ip++], a);
            printf("jmp %s\n", a);
            break;

        case 7:
            data_str(buf[ip++], a);
            data_str(buf[ip++], b);
            printf("jt %s %s\n", a, b);
            break;

        case 8:
            data_str(buf[ip++], a);
            data_str(buf[ip++], b);
            printf("jf %s %s\n", a, b);
            break;

        case 9:
            data_str(buf[ip++], a);
            data_str(buf[ip++], b);
            data_str(buf[ip++], c);
            printf("add %s %s %s\n", a, b, c);
            break;

        case 10:
            data_str(buf[ip++], a);
            data_str(buf[ip++], b);
            data_str(buf[ip++], c);
            printf("mult %s %s %s\n", a, b, c);
            break;

        case 11:
            data_str(buf[ip++], a);
            data_str(buf[ip++], b);
            data_str(buf[ip++], c);
            printf("mod %s %s %s\n", a, b, c);
            break;

        case 12:
            data_str(buf[ip++], a);
            data_str(buf[ip++], b);
            data_str(buf[ip++], c);
            printf("and %s %s %s\n", a, b, c);
            break;

        case 13:
            data_str(buf[ip++], a);
            data_str(buf[ip++], b);
            data_str(buf[ip++], c);
            printf("or %s %s %s\n", a, b, c);
            break;

        case 14:
            data_str(buf[ip++], a);
            data_str(buf[ip++], b);
            printf("not %s %s\n", a, b);
            break;

        case 15:
            data_str(buf[ip++], a);
            data_str(buf[ip++], b);
            printf("rmem %s %s\n", a, b);
            break;

        case 16:
            data_str(buf[ip++], a);
            data_str(buf[ip++], b);
            printf("wmem %s %s\n", a, b);
            break;

        case 17:
            data_str(buf[ip++], a);
            printf("call %s\n", a);
            break;

        case 18:
            printf("ret\n");
            break;

        case 19:
            char_str(buf[ip++], a);
            printf("out %s\n", a);
            break;

        case 20:
            data_str(buf[ip++], a);
            printf("in %s\n", a);
            break;

        case 21:
            printf("noop\n");
            break;

        default:
            raw_str(buf[ip], a);
            printf("DATA %s\n", a);
            break;

        }
    }

    return 0;
}
