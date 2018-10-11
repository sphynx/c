#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INIT_BUFFER_SIZE 4

#ifdef USE_MY_GETLINE
#define GETLINE my_getline
#else
#define GETLINE getline
#endif

#ifdef USE_MY_GETLINE
static ssize_t
go(char **buf, size_t *size, char *curr, size_t read_chars, size_t remaining_chars, FILE *stream) {
    // buf: contains a pointer to a pointer to the first character of a buffer
    //      It is a pointer to pointer, because we want to be able to change it in callee.
    // size: is a pointer to integer, due to the same reason, we want to be able to change
    //      size when we reallocate the buffer.
    // curr: it is a pointer to the current slot, i.e. to the place we are supposed to put
    //      a newly read character in.
    // read_chars: # of chars we've read so far
    // remaining_chars: it is a number of characters we can read to fill the current buffer
    //      (including '\0', for which we also need space).
    int c;
    int sz;

    while (remaining_chars > 1) {
        c = fgetc(stream);
        if (c == '\n') {
            *curr = c;
            curr++;
            // we have read the line, so we should wrap up and stop
            // this loop, no need to realloc:
            *curr = '\0';
            read_chars++;
            break;
        } else if (c != EOF) {
            // some normal character.
            *curr = c;
            curr++;
            read_chars++;
            remaining_chars--;
        } else {
            // EOF or error
            if (read_chars == 0) {
                return -1;
            } else {
                *curr = '\0';
                return read_chars;
            }
        }
    }

    if (remaining_chars == 1) {
        // this means we are out of space (just one slot left for
        // '\0'), so we need to realloc and continue from there
        *buf = (char*) realloc(*buf, (*size) * 2 * sizeof(char)); // twice bigger
        if (*buf == NULL) {
            printf("no memory left\n");
            exit(1);
        }

        // this is a tricky update, might be buggy
        sz = *size;
        curr = (*buf) + read_chars;
        *size = (*size) * 2;
        read_chars = go(buf, size, curr, read_chars, *size - read_chars, stream);
    }

    return read_chars;
}

static ssize_t
my_getline(char **buffer, size_t *size, FILE *stream) {
    if (buffer == NULL || size == NULL) {
        return -1;
    }

    if (*buffer == NULL) {
        // allocate a new buffer and update its size
        *buffer = (char *) malloc(INIT_BUFFER_SIZE * sizeof(char));
        if (*buffer == NULL) {
            printf("no memory left");
            exit(1);
        }
        *size = INIT_BUFFER_SIZE;
    }

    return go(buffer, size, *buffer, 0, *size, stream);
}
#endif

static bool
match(char *term, char *line) {
    return strstr(line, term) != NULL;
}

static void
grep(char *term, FILE *stream) {
    char *buffer = NULL;
    size_t buffer_size = 0;
    ssize_t chars_read;

    while ((chars_read = GETLINE(&buffer, &buffer_size, stream)) > 0) {
        // printf("buffer size = %zu\n", buffer_size);
        // printf("chars read = %zd\n", chars_read);
        if (match(term, buffer)) {
            fwrite(buffer, chars_read, 1, stdout);
        }
    }

    free(buffer);

    // It finished, but not EOF, this means error
    if (!feof(stream)) {
        perror("grep");
    }
}

int
main(int argc, char* argv[]) {
    FILE *stream;
    char *search_term;
    int i;

    if (argc <= 1) {
        printf("my-grep: searchterm [file ...]\n");
        exit(1);
    } else {
        search_term = argv[1];
        if (argc == 2) {
            grep(search_term, stdin);
        } else {
            for (i = 2; i < argc; i++) {
                stream = fopen(argv[i], "r");
                if (stream == NULL) {
                    perror(argv[i]);
                    exit(1);
                }
                grep(search_term, stream);
                fclose(stream);
            }
        }
    }

    return 0;
}
