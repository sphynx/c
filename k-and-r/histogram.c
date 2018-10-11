#include <stdio.h>

#define DIGITS 10
#define LETTERS 26

void
char_entry(char c, int n) {
    int j;

    putchar(c);
    putchar(' ');
    for (j = 0; j < n; ++j)
        putchar('.');
    putchar('\n');
}

int
main() {
    int digits[DIGITS];
    int letters[LETTERS];
    int semicolon, colon, period, comma, dash, lb,
        rb, lp, rp, sq, dq, lt, gt, eq, hash;
    int c, i, j;

    semicolon = colon = period = comma = dash = lb = rb = lp =
        rp = sq = dq = lt = gt = eq = hash = 0;

    for (i = 0; i < DIGITS; ++i)
        digits[i] = 0;

    for (i = 0; i < LETTERS; ++i)
        letters[i] = 0;

    while ((c = getchar()) != EOF) {
        if (c >= '0' && c <= '9')
            digits[c - '0']++;
        else if (c >= 'a' && c <= 'z')
            letters[c - 'a']++;
        else if (c >= 'A' && c <= 'Z')
            letters[c - 'A']++;
        else if (c == ';')
            semicolon++;
        else if (c == ':')
            colon++;
        else if (c == '.')
            period++;
        else if (c == ',')
            comma++;
        else if (c == '-')
            dash++;
        else if (c == '[')
            lb++;
        else if (c == ']')
            rb++;
        else if (c == '(')
            lp++;
        else if (c == ')')
            rp++;
        else if (c == '\'')
            sq++;
        else if (c == '\"')
            dq++;
        else if (c == '<')
            lt++;
        else if (c == '>')
            gt++;
        else if (c == '=')
            eq++;
        else if (c == '#')
            hash++;
    }

    for (i = 0; i < DIGITS; ++i) {
        putchar('0' + i);
        putchar(' ');
        for (j = 0; j < digits[i]; ++j)
            putchar('.');
        putchar('\n');
    }

    for (i = 0; i < LETTERS; ++i) {
        putchar('A' + i);
        putchar(' ');
        for (j = 0; j < letters[i]; ++j)
            putchar('.');
        putchar('\n');
    }

    char_entry(';', semicolon);
    char_entry(':', colon);
    char_entry('.', period);
    char_entry(',', comma);
    char_entry('-', dash);
    char_entry('[', lb);
    char_entry(']', rb);
    char_entry('(', lp);
    char_entry(')', rp);
    char_entry('\'', sq);
    char_entry('\"', dq);
    char_entry('<', lt);
    char_entry('>', gt);
    char_entry('=', eq);
    char_entry('#', hash);
}
