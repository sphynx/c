%option noinput
%option nounput

%{
#include <inttypes.h>

#include "asm.tab.h"

extern YYSTYPE yylval;
%}

%%
%[0-7]     { yylval = (uint16_t) atoi(yytext + 1); return REG; }
[0-9]+     { yylval = (uint16_t) atoi(yytext); return NUM; }
'.'        { yylval = (uint16_t) (yytext[1]); return CHAR; }
"halt"     { return HALT; }
"push"     { return PUSH; }
"pop"      { return POP; }
"set"      { return SET; }
[ \t]      ;
\n         return EOL;
.          return yytext[0];
%%

