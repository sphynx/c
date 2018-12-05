#pragma once

#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>

void gen_code(FILE *out, uint16_t op_code, int arity, ...);
void dw(FILE *out, uint16_t word);
uint16_t current_addr(void);
void gen_reset(void);
