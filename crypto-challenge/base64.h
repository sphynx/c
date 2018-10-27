#pragma once

#include <inttypes.h>
#include <stdio.h>

/*
 * Encodes to BASE64, returns a pointer to a newly allocated
 * zero-terminated string or NULL in case of error. Don't forget to
 * free() that string later.
 */
char* base64_encode(uint8_t* from, size_t len);

/*
 * Accept a NUL-terminated string, decodes it as BASE64, returns a
 * pointer to a newly allocated byte array or NULL in case of error.
 * Writes the size of the newly allocated array to *len.
 */
uint8_t* base64_decode(char* from, size_t* len);

