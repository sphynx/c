#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#define MEMORY_SIZE (1ULL << 15)
#define STACK_SIZE 1024

// VM state.
static uint16_t mem[MEMORY_SIZE] = {0};
static uint16_t regs[8] = {0};
static uint16_t stack[STACK_SIZE] = {0};
static uint16_t ip = 0; // only lowest 15 bits can address memory
static uint16_t sp = 0; // points to the location for the *next* val

static void err(const char* msg)
{
    fprintf(stderr, "error: %s\n", msg);
}

static int is_literal(uint16_t x)
{
    return x <= 32767;
}

static int is_register(uint16_t x)
{
    return x >= 32768 && x <= 32775;
}

static int reg_no(uint16_t x) {
    return x - 32768;
}

static void set(uint16_t lhs, uint16_t rhs)
{
    if (is_register(lhs)) {
        regs[reg_no(lhs)] = rhs;
    } else if (is_literal(lhs)) {
        err("set(): literal LHS");
        exit(EXIT_FAILURE);
    } else {
        err("set(): invalid number in LHS");
        exit(EXIT_FAILURE);
    }
}

static uint16_t get(uint16_t data)
{
    if (is_register(data))
        return regs[reg_no(data)];

    if (is_literal(data))
        return data;

    err("rhs(): invalid number");
    exit(EXIT_FAILURE);
}


static void vm_run(void)
{
    for (;;) {
        if (ip >= MEMORY_SIZE)
            err("IP out of bounds");

        if (sp >= STACK_SIZE)
            err("stack overflow");

        switch (mem[ip]) {
        case 0:
            // halt
            exit(EXIT_SUCCESS);

        case 1:
            // set
            set(mem[ip + 1], get(mem[ip + 2]));
            ip += 3;
            break;

        case 2:
            // push
            stack[sp++] = get(mem[ip + 1]);
            ip += 2;
            break;

        case 3:
            // pop
            if (sp == 0) {
                err("pop: stack underflow");
                exit(EXIT_FAILURE);
            }
            set(mem[ip + 1], stack[--sp]);
            ip += 2;
            break;

        case 4:
            // eq
            set(mem[ip + 1], get(mem[ip + 2]) == get(mem[ip + 3]));
            ip += 4;
            break;

        case 5:
            // gt
            set(mem[ip + 1], get(mem[ip + 2]) > get(mem[ip + 3]));
            ip += 4;
            break;

        case 6:
            // jmp
            ip = get(mem[ip + 1]);
            break;

        case 7:
            // jt
            if (get(mem[ip + 1]) != 0)
                ip = get(mem[ip + 2]);
            else
                ip += 3;
            break;

        case 8:
            // jf
            if (get(mem[ip + 1]) == 0)
                ip = get(mem[ip + 2]);
            else
                ip += 3;
            break;

        case 9:
            // add
            set(mem[ip + 1], (get(mem[ip + 2]) + get(mem[ip + 3])) % 32768);
            ip += 4;
            break;

        case 10: {
            // mult
            uint32_t x = (uint32_t) get(mem[ip + 2]);
            uint32_t y = (uint32_t) get(mem[ip + 3]);
            uint16_t m = (uint16_t) ((x * y) % 32768);
            set(mem[ip + 1], m);
            ip += 4;
            break;
        }

        case 11:
            // mod
            set(mem[ip + 1], get(mem[ip + 2]) % get(mem[ip + 3]));
            ip += 4;
            break;

        case 12:
            // and
            set(mem[ip + 1], get(mem[ip + 2]) & get(mem[ip + 3]));
            ip += 4;
            break;

        case 13:
            // or
            set(mem[ip + 1], get(mem[ip + 2]) | get(mem[ip + 3]));
            ip += 4;
            break;

        case 14: {
            // not
            uint16_t x = get(mem[ip + 2]);
            uint16_t nx = (~x) & 0x7FFF;
            set(mem[ip + 1], nx);
            ip += 3;
            break;
        }

        case 15:
            // rmem
            set(mem[ip + 1], mem[get(mem[ip + 2])]);
            ip += 3;
            break;

        case 16:
            // wmem
            mem[get(mem[ip + 1])] = get(mem[ip + 2]);
            ip += 3;
            break;

        case 17:
            // call
            stack[sp++] = ip + 2;
            ip = get(mem[ip + 1]);
            break;

        case 18:
            // ret
            if (sp == 0)
                exit(EXIT_SUCCESS);

            ip = stack[--sp];
            break;

        case 19:
            // out
            putchar(get(mem[ip + 1]));
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

            set(mem[ip + 1], (uint32_t) c);
            ip += 2;
            break;
        }

        case 21:
            // noop
            ip += 1;
            break;

        default:
            fprintf(stderr, "error: unknown op-code %" PRIu16 "\n",
                    mem[ip]);
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char* argv[])
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

    size_t prog_len = fread(&mem, 16, MEMORY_SIZE, in);
    if (prog_len == 0) {
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
