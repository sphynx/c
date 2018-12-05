#include <stdio.h>
#include <stdarg.h>

#include "codegen.h"

static uint16_t ip = 0;

static void put(FILE *out, uint16_t x)
{
    if (out != NULL)
        fwrite(&x, 2, 1, out);
    ip++;
}

void gen_code(FILE *out, uint16_t op_code, int arity, ...)
{
    va_list ap;
    put(out, op_code);

    va_start(ap, arity);
    for (int i = 0; i < arity; i++) {
        int arg = va_arg(ap, int);
        uint16_t operand = (uint16_t) arg;
        put(out, operand);
    }

    va_end(ap);
}

void dw(FILE *out, uint16_t word)
{
    put(out, word);
}

uint16_t current_addr(void)
{
    return ip;
}

void gen_reset(void)
{
    ip = 0;
}
