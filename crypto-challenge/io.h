#pragma once

#include <stdio.h>
#include <inttypes.h>

/*
 * Reads whole contents of file as a byte buffer. Allocates a new
 * buffer with the size equal to the file length in bytes (you have to
 * free it). Writes a size of that buffer in *len. Returns a pointer
 * to that new buffer. Under the good it uses fseek() to determine the
 * file size, so it won't work on streams not supporting seek(), e.g.
 * pipes. Returns NULL and prints an error message if anything goes
 * wrong.
 */
uint8_t* get_file_contents(FILE* stream, size_t* len);


/*
 * Reads all non-space characters from the stream (skipping SPC, TAB
 * and end of lines), returns a newly allocated NUL-terminated string
 * or NULL if something goes wrong.
 */
char* read_non_space(FILE* stream);
