#include <stdio.h>

#include "codegen.h"

static void put(uint16_t x)
{
    fwrite(&x, 2, 1, stdout);
}

void halt(void)
{
    uint16_t op_code = 0U;
    put(op_code);
}

void set(uint16_t reg, uint16_t val)
{
    uint16_t op_code = 1U;
    put(op_code);
    put(reg);
    put(val);
}

void push(uint16_t val)
{
    uint16_t op_code = 2U;
    put(op_code);
    put(val);
}

void pop(uint16_t reg)
{
    uint16_t op_code = 3U;
    put(op_code);
    put(reg);
}
