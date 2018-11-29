#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

#define STACK_SIZE 1024

// VM state.
static uint16_t mem[MEMORY_SIZE] = {0};
static uint16_t regs[8] = {0};
static uint16_t stack[STACK_SIZE] = {0};
static uint16_t ip = 0; // only lowest 15 bits can address memory
static uint16_t sp = 0; // points to the location for the *next* push

static void
set_reg(uint16_t lhs, uint16_t rhs)
{
    if (is_register(lhs)) {
        regs[reg_no(lhs)] = rhs;
    } else if (is_literal(lhs)) {
        err("set_reg(): literal LHS");
        exit(EXIT_FAILURE);
    } else {
        err("set_reg(): invalid number in LHS");
        exit(EXIT_FAILURE);
    }
}

static uint16_t
reg_or_lit(uint16_t data)
{
    if (is_register(data))
        return regs[reg_no(data)];

    if (is_literal(data))
        return data;

    err("reg_or_lit(): invalid number");
    exit(EXIT_FAILURE);
}

static void
vm_run(void)
{
    for (;;) {
        if (ip >= MEMORY_SIZE) {
            err("IP out of bounds");
            exit(EXIT_FAILURE);
        }

        if (sp >= STACK_SIZE) {
            err("stack overflow");
            exit(EXIT_FAILURE);
        }

        switch (mem[ip]) {
        case 0:
            // halt
            exit(EXIT_SUCCESS);

        case 1:
            // set
            set_reg(mem[ip + 1], reg_or_lit(mem[ip + 2]));
            ip += 3;
            break;

        case 2:
            // push
            stack[sp++] = reg_or_lit(mem[ip + 1]);
            ip += 2;
            break;

        case 3:
            // pop
            if (sp == 0) {
                err("pop: stack underflow");
                exit(EXIT_FAILURE);
            }
            set_reg(mem[ip + 1], stack[--sp]);
            ip += 2;
            break;

        case 4:
            // eq
            set_reg(mem[ip + 1],
                    reg_or_lit(mem[ip + 2]) == reg_or_lit(mem[ip + 3]));
            ip += 4;
            break;

        case 5:
            // gt
            set_reg(mem[ip + 1],
                    reg_or_lit(mem[ip + 2]) > reg_or_lit(mem[ip + 3]));
            ip += 4;
            break;

        case 6:
            // jmp
            ip = reg_or_lit(mem[ip + 1]);
            break;

        case 7:
            // jt
            if (reg_or_lit(mem[ip + 1]) != 0)
                ip = reg_or_lit(mem[ip + 2]);
            else
                ip += 3;
            break;

        case 8:
            // jf
            if (reg_or_lit(mem[ip + 1]) == 0)
                ip = reg_or_lit(mem[ip + 2]);
            else
                ip += 3;
            break;

        case 9:
            // add
            set_reg(mem[ip + 1],
                    (reg_or_lit(mem[ip + 2]) + reg_or_lit(mem[ip + 3])) % 32768);
            ip += 4;
            break;

        case 10: {
            // mult
            uint32_t x = (uint32_t) reg_or_lit(mem[ip + 2]);
            uint32_t y = (uint32_t) reg_or_lit(mem[ip + 3]);
            uint16_t m = (uint16_t) ((x * y) % 32768);
            set_reg(mem[ip + 1], m);
            ip += 4;
            break;
        }

        case 11: {
            // mod
            uint16_t d = reg_or_lit(mem[ip + 3]);
            if (d == 0) {
                err("mod: division by zero");
                exit(EXIT_FAILURE);
            }

            set_reg(mem[ip + 1], reg_or_lit(mem[ip + 2]) % d);
            ip += 4;
            break;
        }

        case 12:
            // and
            set_reg(mem[ip + 1],
                    reg_or_lit(mem[ip + 2]) & reg_or_lit(mem[ip + 3]));
            ip += 4;
            break;

        case 13:
            // or
            set_reg(mem[ip + 1],
                    reg_or_lit(mem[ip + 2]) | reg_or_lit(mem[ip + 3]));
            ip += 4;
            break;

        case 14: {
            // not
            uint16_t x = reg_or_lit(mem[ip + 2]);
            uint16_t nx = (~x) & 0x7FFF;
            set_reg(mem[ip + 1], nx);
            ip += 3;
            break;
        }

        case 15:
            // rmem
            set_reg(mem[ip + 1], mem[reg_or_lit(mem[ip + 2])]);
            ip += 3;
            break;

        case 16:
            // wmem
            mem[reg_or_lit(mem[ip + 1])] = reg_or_lit(mem[ip + 2]);
            ip += 3;
            break;

        case 17:
            // call
            stack[sp++] = ip + 2;
            ip = reg_or_lit(mem[ip + 1]);
            break;

        case 18:
            // ret
            if (sp == 0)
                exit(EXIT_SUCCESS);

            ip = stack[--sp];
            break;

        case 19:
            // out
            putchar(reg_or_lit(mem[ip + 1]));
            ip += 2;
            break;

        case 20: {
            // in
            int c = getchar();

            if (ferror(stdin)) {
                perror("getchar");
                exit(EXIT_FAILURE);
            }

            if (feof(stdin)) {
                err("no input provided");
                exit(EXIT_FAILURE);
            }

            set_reg(mem[ip + 1], (uint16_t) c);
            ip += 2;
            break;
        }

        case 21:
            // noop
            ip += 1;
            break;

        default:
            fprintf(stderr, "error: unknown op-code %" PRIu16 "\n", mem[ip]);
            exit(EXIT_FAILURE);
        }
    }
}

int
main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s <binary_to_run>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE* in = fopen(argv[1], "rb");
    if (in == NULL) {
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }

    if (fread(mem, 2, MEMORY_SIZE, in) == 0) {
        if (feof(in)) {
            err("empty program provided");
            exit(EXIT_FAILURE);
        }

        if (ferror(in)) {
            perror("fread");
            exit(EXIT_FAILURE);
        }
    }

    vm_run();

    return 0;
}
