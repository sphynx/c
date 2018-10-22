#pragma once

#include <inttypes.h>
#include <stdio.h>

uint8_t* read_hex(char *hex_str, size_t len);
char* write_hex(uint8_t* bytes, size_t len);
