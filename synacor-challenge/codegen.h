#pragma once

#include <inttypes.h>
#include <stdarg.h>

void gen_code(uint16_t op_code, int arity, ...);
void dw(uint16_t word);
uint16_t current_addr(void);
