#include <stdio.h>

#include "lexer.h"

enum token_type
next_token(char **input, char *token)
{
    int matched_chars = 0;
    char c = **input;

    // End of input
    if (c == '\0') {
        return END;
    }

    // GT.
    if (c == '>') {
        *input += 1;
        return GT;
    }

    // Word.
    sscanf(*input, "%[^> \n\t]%n", token, &matched_chars);
    if (matched_chars > 0) {
        *input += matched_chars;
        return WORD;
    }

    // Space.
    while (c == ' ' || c == '\t' || c == '\n') {
        *input += 1;
        c = **input;
    }
    return SPACE;
}




