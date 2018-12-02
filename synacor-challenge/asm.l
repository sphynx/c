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
'\\n'      { yylval = 10; return CHAR; }
"halt"     { return HALT; }
"set"      { return SET; }
"push"     { return PUSH; }
"pop"      { return POP; }
"eq"       { return EQ; }
"gt"       { return GT; }
"jmp"      { return JMP; }
"jt"       { return JT; }
"jf"       { return JF; }
"add"      { return ADD; }
"mult"     { return MULT; }
"mod"      { return MOD; }
"and"      { return AND; }
"or"       { return OR; }
"not"      { return NOT; }
"rmem"     { return RMEM; }
"wmem"     { return WMEM; }
"call"     { return CALL; }
"ret"      { return RET; }
"out"      { return OUT; }
"in"       { return IN; }
"noop"     { return NOOP; }
"dw"       { return DW; }
[ \t]      ;
;[^\n]*    ;
\n         return EOL;
.          return yytext[0];
%%

