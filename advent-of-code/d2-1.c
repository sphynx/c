#include <stdio.h>
#include <string.h>

int main(void)
{
    int twos       = 0;
    int seen_two   = 0;
    int threes     = 0;
    int seen_three = 0;

    char line[30];
    int freq[26] = {0};
    char *curr = line;

    while (fgets(line, 30, stdin)) {
        while (*curr != '\0' && *curr != '\n')
            freq[*curr++ - 'a']++;
            curr++;

        for (int i = 0; i < 26; i++) {
            switch (freq[i]) {
            case 2: if (!seen_two) twos++; seen_two = 1; break;
            case 3: if (!seen_three) threes++; seen_three = 1; break;
            }
        }

        seen_two = 0;
        seen_three = 0;
        memset(freq, 0, sizeof(freq));
        curr = line;
    }

    printf("2s: %d, 3s: %d, result: %d\n", twos, threes, twos * threes);

    return 0;
}
