#include <stdlib.h>

#include "endian.h"

uint8_t
*bytes_le_order(uint64_t in)
{
    uint8_t *res_ptr = malloc(8);
    uint8_t *in_ptr = (uint8_t *) &in;

    for (int i = 0; i < 8; i++)
        res_ptr[i] = is_big_endian() ? in_ptr[8-i] : in_ptr[i];

    return res_ptr;
}
