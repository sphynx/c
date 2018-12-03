#include <stdio.h>
#include <stdarg.h>

#include "codegen.h"

static uint16_t ip = 0;

static void put(uint16_t x)
{
    fwrite(&x, 2, 1, stdout);
    ip++;
}

void gen_code(uint16_t op_code, int arity, ...)
{
    va_list ap;
    put(op_code);

    va_start(ap, arity);
    for (int i = 0; i < arity; i++) {
        int arg = va_arg(ap, int);
        uint16_t operand = (uint16_t) arg;
        put(operand);
    }

    va_end(ap);
}

void dw(uint16_t word)
{
    put(word);
}

uint16_t current_addr(void)
{
    return ip;
}
