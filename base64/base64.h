#pragma once

#include <inttypes.h>
#include <stdio.h>

/*
 * Encodes to BASE64, returns a pointer to a newly allocated
 * zero-terminated string or NULL in case of error. Don't forget to
 * free() that string later.
 */
char* base64_encode(uint8_t* from, size_t len);

