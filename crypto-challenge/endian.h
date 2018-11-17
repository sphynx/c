#pragma once

#include <inttypes.h>

static const uint16_t be_test = 1;
#define is_big_endian() ((* (char*) &be_test) == 0)

/*
 * Returns bytes in little endian order (i.e. smallest first)
 */
uint8_t *bytes_le_order(uint64_t in);
