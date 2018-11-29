#pragma once

#include <inttypes.h>

#define MEMORY_SIZE (1ULL << 15)

void err(const char* msg);

int is_register(uint16_t x);

int is_literal(uint16_t x);

int reg_no(uint16_t x);
