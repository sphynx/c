#include <stdlib.h>
#include <string.h>

#include "io.h"
#include "xor.h"

int main(int argc, char* argv[]) {

    size_t num_bytes;
    uint8_t* buffer;
    uint8_t* encoded_buffer;

    if (argc < 2) {
        fprintf(stderr, "usage: %s <key> encodes stdin\n", argv[0]);
        exit(1);
    }

    if (argc == 2) {
        char* key = argv[1];
        buffer = get_file_contents(stdin, &num_bytes);

        if (buffer == NULL) {
            fprintf(stderr, "couldn't read stdin");
            exit(1);
        }

        encoded_buffer = xor_with_repeating_key(buffer, num_bytes, (uint8_t*) key, strlen(key));

        if (fwrite(encoded_buffer, 1, num_bytes, stdout) != num_bytes) {
            perror("fwrite");
            exit(1);
        }

        free(buffer);
        free(encoded_buffer);
    }

    return 0;
}
