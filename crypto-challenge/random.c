#include <stdlib.h>
#include "random.h"

void
init_with_random_bytes(unsigned char *data, size_t data_len)
{
    arc4random_buf(data, data_len);
}
