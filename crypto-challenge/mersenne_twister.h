#pragma once

#include <stdint.h>

void seed(uint32_t seed);

uint32_t extract(void);

void set_state(uint32_t *new_state);
