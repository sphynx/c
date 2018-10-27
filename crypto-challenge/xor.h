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
 * Returns bitwise xor of two buffers represented as hex strings.
 * Result will also be encoded as a hex string. The resulting string
 * is malloc-ed, it's responsibility of the caller to free it.
 */
char* xor_hex(char* s1, char* s2);

/*
 * Returns bitwise xor of a single byte key and provided byte message.
 * The resulting byte buffer is malloc-ed, it's responsibility of the
 * caller to free it.
 */
uint8_t* xor_with_byte_key(uint8_t* msg, uint8_t key, size_t len);

/*
 * The same as `xor_with_byte_key` but allows to xor only bytes
 * starting at `start` and progressing with `step`. i.e. if start=3
 * and step=2 we only encode with xor the following bytes: 3, 5, 7, 9,
 * etc. The rest is left unchanged.
 */
uint8_t* xor_with_byte_key_step(uint8_t* msg, uint8_t key, size_t len,
                                size_t start, size_t step);

/*
 * Returns bitwise xor of a single byte key and provided byte message.
 * The resulting byte buffer is malloc-ed, it's responsibility of the
 * caller to free it.
 */
uint8_t* xor_with_repeating_key(uint8_t* msg, size_t msg_len,
                                uint8_t* key, size_t key_len);


