#include <assert.h>
#include <string.h>

#include "pkcs7-padding.h"

static size_t
calc_padding_len(size_t data_len, size_t block_len) {
    return block_len - (data_len % block_len);
}

ssize_t
pkcs7_pad(uint8_t* data, size_t data_len, size_t block_len,
          uint8_t** padded_data) {

    size_t padding_len = calc_padding_len(data_len, block_len);
    size_t padded_len = data_len + padding_len;
    uint8_t* new_data;
    size_t i;

    assert(data != NULL);
    assert(data_len > 0);
    assert(block_len > 0 && block_len < 256);
    assert(padded_data != NULL);

    new_data = malloc(padded_len);
    if (new_data == NULL) {
        return -1;
    }

    (void) memcpy(new_data, data, data_len);

    for (i = data_len; i < padded_len; i++) {
        new_data[i] = padding_len;
    }

    *padded_data = new_data;
    return padded_len;
}
