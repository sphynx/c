/*
 * Basic BASE64 encoder.
 * Implementation is based on https://en.wikipedia.org/wiki/Base64
 * Solution to http://cryptopals.com/sets/1/challenges/1
 * MIME version.
 */

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned char sextet;
typedef unsigned char octet;
typedef unsigned char uchar;

long
base64_encoded_length(long len) {
    return (long) (ceil((double) len / 3.0) * 4.0);
}

char
encode_char(sextet sextet) {
    char res;

    assert(sextet < 64);

    if (sextet < 26) {
        // 'A' .. 'Z'
        res = 'A' + sextet;
    } else if (sextet < 52) {
        // 'a' .. 'z'
        res = 'a' + (sextet - 26);
    } else if (sextet < 62) {
        // '0' .. '9'
        res = '0' + (sextet - 52);
    } else if (sextet == 62) {
        res = '+';
    } else {
        res = '/';
    }
    return res;
}

void
encode_1byte(octet byte1, char* encoded) {
    sextet sextets[2];

    sextets[0] = byte1 >> 2;
    sextets[1] = (byte1 & 0x03) << 4;

    *encoded++ = encode_char(sextets[0]);
    *encoded++ = encode_char(sextets[1]);
    *encoded++ = '=';
    *encoded   = '=';
}

void
encode_2bytes(octet byte1, octet byte2, char* encoded) {
    int i;
    sextet sextets[3];

    sextets[0] = byte1 >> 2;
    sextets[1] = ((byte1 & 0x03) << 4) | (byte2 >> 4);
    sextets[2] = (byte2 & 0x0F) << 2;

    for (i = 0; i < 3; i++) {
        *encoded++ = encode_char(sextets[i]);
    }
    *encoded = '=';
}

void
encode_3bytes(octet byte1, octet byte2, octet byte3, char* encoded) {
    int i;
    sextet sextets[4];

    sextets[0] = byte1 >> 2;
    sextets[1] = ((byte1 & 0x03) << 4) | (byte2 >> 4);
    sextets[2] = ((byte2 & 0x0F) << 2) | (byte3 >> 6);
    sextets[3] = byte3 & 0x3F;

    for (i = 0; i < 4; i++) {
        *encoded++ = encode_char(sextets[i]);
    }
}


/*
 * Encodes to BASE64 and returns number of bytes succesfully encoded.
 */
long
base64_encode(octet* from, long len, char* to) {
    int i;
    long padded_bytes;
    octet byte1, byte2, byte3;

    // Now, we handle 3 bytes at a time, converting them into 4
    // sextets.
    for (i = 0; i < len / 3; i++) {
        byte1 = *from++;
        byte2 = *from++;
        byte3 = *from++;
        encode_3bytes(byte1, byte2, byte3, to);
        to += 4;
    }

    // Convert leftover bytes (if any) using padding.
    padded_bytes = len - 3 * (len / 3);
    if (padded_bytes == 1) {
        encode_1byte(*from, to);
    } else if (padded_bytes == 2) {
        encode_2bytes(*from, *(from + 1), to);
    }

    return len;
}

int
main(int argc, char* argv[]) {
    FILE* stream;

    long num_bytes;
    uchar* buffer;

    long num_encoded_bytes;
    char* encoded_buffer;

    if (argc == 2) {
        stream = fopen(argv[1], "rb");
        if (stream == NULL) {
            perror(argv[1]);
            exit(1);
        }

        // seek to end to determine file size
        if (fseek(stream, 0, SEEK_END) == -1) {
            perror("fseek");
            exit(1);
        }

        num_bytes = ftell(stream);

        // rewind back to the beginning
        if (fseek(stream, 0, SEEK_SET) == -1) {
            perror("fseek");
            exit(1);
        }

        buffer = (uchar*) malloc(num_bytes);
        fread(buffer, 1, num_bytes, stream);

        num_encoded_bytes = base64_encoded_length(num_bytes);
        encoded_buffer = (char*) malloc(num_encoded_bytes);
        base64_encode(buffer, num_bytes, encoded_buffer);
        fwrite(encoded_buffer, 1, num_encoded_bytes, stdout);
    } else {
        printf("usage: base64 <file>\n");
        exit(1);
    }

    return 0;
}
