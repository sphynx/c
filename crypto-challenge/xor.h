#pragma once

#include <stdio.h>
#include <inttypes.h>

/*
 * Returns bitwise xor of two byte buffers of the same length. The
 * resulting byte buffer is malloc-ed, it's responsibility of the
 * caller to free it.
 */
uint8_t* xor(uint8_t* b1, uint8_t* b2, size_t len);

/*
 * Returns bitwise xor of a single byte key and provided byte buffer.
 * The resulting byte buffer is malloc-ed, it's responsibility of the
 * caller to free it.
 */
uint8_t* xor_with_byte(uint8_t* buf, uint8_t key, size_t len);

/*
 * Returns bitwise xor of two buffers represented as hex strings.
 * Result will also be encoded as a hex string. The resulting string
 * is malloc-ed, it's responsibility of the caller to free it.
 */
char* xor_hex(char* s1, char* s2);

