#include <assert.h>
#include <inttypes.h>
#include <string.h>

#include "pkcs7-padding.h"

int main(void)
{
    char *good = "ICE ICE BABY\x04\x04\x04\x04";
    char *bad1 = "ICE ICE BABY\x05\x05\x05\x05";
    char *bad2 = "ICE ICE BABY\x01\x02\x03\x04";

    assert(padding_len((uint8_t*) good, strlen(good)) == 4);
    assert(padding_len((uint8_t*) bad1, strlen(bad1)) == -1);
    assert(padding_len((uint8_t*) bad2, strlen(bad2)) == -1);

    // Stripping the padding amounts to just treating the byte string
    // as a shorter buffer (length = orig_len - padding_len(...)). We
    // can also allocate a smaller buffer for it, copy it there and
    // free the original one. But in general the string can be
    // allocated on stack as well, so we don't provide any generic
    // function for stripping.

    return 0;
}
