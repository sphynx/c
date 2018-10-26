#pragma once

#include <inttypes.h>
#include <stdio.h>


/*
 * Accepts null-terminated hex-string (e.g. "4AE6" or "4ea6") which
 * should contain even number of hex digits. Returns a corresponding
 * malloc-ed byte buffer, its length is half of the length of the
 * input string (excluding 0-terminator).
 */
uint8_t* read_hex(char *hex_str, size_t len);

char* write_hex(uint8_t* bytes, size_t len);
