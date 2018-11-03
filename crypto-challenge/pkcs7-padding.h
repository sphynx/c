#pragma once

#include <inttypes.h>
#include <stdlib.h>

/*
 * Accepts a byte string, its length and block_len. Pads the byte
 * string, allocates a new buffer to contain the string and the
 * padding, copies data, appends padding bytes and writes the result
 * to *padded_data. Returns size of the padded block or -1 if
 * something goes wrong.
 */
ssize_t
add_padding(uint8_t* data, size_t data_len, size_t block_len,
            uint8_t** padded_data);


/*
 * Accepts a byte string and its length. Determines how much padding is
 * added to the string and returns number of bytes to be trimmed from
 * the end of the string or -1 if something goes wrong.
 */
ssize_t
padding_len(uint8_t* padded_data, size_t data_len);
