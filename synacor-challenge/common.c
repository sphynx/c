#include <stdio.h>

#include "common.h"

void
err(const char* msg)
{
    fprintf(stderr, "error: %s\n", msg);
}

int
is_literal(uint16_t x)
{
    return x <= 32767;
}

int
is_register(uint16_t x)
{
    return x >= 32768 && x <= 32775;
}

int
is_value(uint16_t x)
{
    return x <= 32775;
}

int
reg_no(uint16_t x)
{
    return x - 32768;
}
