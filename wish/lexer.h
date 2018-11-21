#pragma once

enum token_type { WORD, SPACE, GT, END };

enum token_type next_token(char **input, char *token);
