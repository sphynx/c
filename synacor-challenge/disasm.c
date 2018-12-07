#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#define REG(n) (is_register(buf[ip + n]))
#define LIT(n) (is_literal(buf[ip + n]))
#define VAL(n) (is_value(buf[ip + n]))

#define COMMENT_OFFSET 25

static uint16_t buf[MEMORY_SIZE];

static void
val_str(const uint16_t data, char *str)
{
    assert(is_value(data));
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
    assert(is_value(data));
    if (is_register(data)) {
        snprintf(str, 8, "%%%d", reg_no(data));
    } else {
        if (data == 10) {
            snprintf(str, 8, "'\\n'");
        } else if (isprint(data)) {
            snprintf(str, 8, "'%c'", (char) data);
        } else {
            snprintf(str, 8, "%" PRIu16, data);
        }
    }
}

static int
check_instruction(const uint16_t ip)
{
    switch (buf[ip]) {
    case  0: return 1;
    case  1: return REG(1) && VAL(2);
    case  2: return VAL(1);
    case  3: return REG(1);
    case  4: return REG(1) && VAL(2) && VAL(3);
    case  5: return REG(1) && VAL(2) && VAL(3);
    case  6: return VAL(1);
    case  7: return VAL(1) && VAL(2);
    case  8: return VAL(1) && VAL(2);
    case  9: return REG(1) && VAL(2) && VAL(3);
    case 10: return REG(1) && VAL(2) && VAL(3);
    case 11: return REG(1) && VAL(2) && VAL(3);
    case 12: return REG(1) && VAL(2) && VAL(3);
    case 13: return REG(1) && VAL(2) && VAL(3);
    case 14: return REG(1) && VAL(2);
    case 15: return REG(1) && VAL(2);
    case 16: return REG(1) && VAL(2);
    case 17: return VAL(1);
    case 18: return 1;
    case 19: return VAL(1);
    case 20: return REG(1);
    case 21: return 1;
    default: return 0;
    }
}

int
main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s <vm_program> (use '-' for stdin)\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE *in;
    if (strcmp(argv[1], "-") == 0) {
        in = stdin;
    } else {
        in = fopen(argv[1], "rb");
        if (in == NULL) {
            perror(argv[1]);
            exit(EXIT_FAILURE);
        }
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
    uint16_t old_ip = ip;
    int n;

    char a[8];
    char b[8];
    char c[8];

    for (;;) {
        if (ip >= read_bytes) {
            break;
        }

        if (check_instruction(ip)) {

            switch (buf[ip++]) {
            case 0:
                n = printf("halt");
                break;

            case 1:
                val_str(buf[ip++], a);
                val_str(buf[ip++], b);
                n = printf("set %s %s", a, b);
                break;

            case 2:
                val_str(buf[ip++], a);
                n = printf("push %s", a);
                break;

            case 3:
                val_str(buf[ip++], a);
                n = printf("pop %s", a);
                break;

            case 4:
                val_str(buf[ip++], a);
                val_str(buf[ip++], b);
                val_str(buf[ip++], c);
                n = printf("eq %s %s %s", a, b, c);
                break;

            case 5:
                val_str(buf[ip++], a);
                val_str(buf[ip++], b);
                val_str(buf[ip++], c);
                n = printf("gt %s %s %s", a, b, c);
                break;

            case 6:
                val_str(buf[ip++], a);
                n = printf("jmp %s", a);
                break;

            case 7:
                val_str(buf[ip++], a);
                val_str(buf[ip++], b);
                n = printf("jt %s %s", a, b);
                break;

            case 8:
                val_str(buf[ip++], a);
                val_str(buf[ip++], b);
                n = printf("jf %s %s", a, b);
                break;

            case 9:
                val_str(buf[ip++], a);
                val_str(buf[ip++], b);
                val_str(buf[ip++], c);
                n = printf("add %s %s %s", a, b, c);
                break;

            case 10:
                val_str(buf[ip++], a);
                val_str(buf[ip++], b);
                val_str(buf[ip++], c);
                n = printf("mult %s %s %s", a, b, c);
                break;

            case 11:
                val_str(buf[ip++], a);
                val_str(buf[ip++], b);
                val_str(buf[ip++], c);
                n = printf("mod %s %s %s", a, b, c);
                break;

            case 12:
                val_str(buf[ip++], a);
                val_str(buf[ip++], b);
                val_str(buf[ip++], c);
                n = printf("and %s %s %s", a, b, c);
                break;

            case 13:
                val_str(buf[ip++], a);
                val_str(buf[ip++], b);
                val_str(buf[ip++], c);
                n = printf("or %s %s %s", a, b, c);
                break;

            case 14:
                val_str(buf[ip++], a);
                val_str(buf[ip++], b);
                n = printf("not %s %s", a, b);
                break;

            case 15:
                val_str(buf[ip++], a);
                val_str(buf[ip++], b);
                n = printf("rmem %s %s", a, b);
                break;

            case 16:
                val_str(buf[ip++], a);
                val_str(buf[ip++], b);
                n = printf("wmem %s %s", a, b);
                break;

            case 17:
                val_str(buf[ip++], a);
                n = printf("call %s", a);
                break;

            case 18:
                n = printf("ret");
                break;

            case 19:
                char_str(buf[ip++], a);
                n = printf("out %s", a);
                break;

            case 20:
                val_str(buf[ip++], a);
                n = printf("in %s", a);
                break;

            case 21:
                n = printf("noop");
                break;
            }

        } else {
            raw_str(buf[ip++], a);
            n = printf("dw %s", a);
        }

        printf("%*c", COMMENT_OFFSET - n, ' ');
        printf("; %" PRIu16 "\n", old_ip);
        old_ip = ip;
    }

    return 0;
}
