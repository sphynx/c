#pragma once

#include <inttypes.h>

void halt(void);
void set(uint16_t reg, uint16_t val);
void push(uint16_t val);
void pop(uint16_t reg);
