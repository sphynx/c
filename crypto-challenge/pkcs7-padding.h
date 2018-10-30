#pragma once

#include <inttypes.h>
#include <stdlib.h>

/*
 * Accepts byte string, its length and block_len. Pads the byte
 * string, allocates a new buffer to contain the string and the
 * padding, copies data, appends padding bytes and writes the result
 * to *padded_data. Returns size of the padded block or -1 if
 * something goes wrong.
 */
ssize_t
pkcs7_pad(uint8_t* data, size_t data_len, size_t block_len,
          uint8_t** padded_data);
