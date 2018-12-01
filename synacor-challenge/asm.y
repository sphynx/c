%{
#include <inttypes.h>
#include <stdio.h>

#include "codegen.h"

extern int yylex(void);
void yyerror(const char *s);

%}

%define api.value.type {uint16_t}

%token EOL
%token REG NUM CHAR
%token HALT PUSH POP SET

%%

instr_list: instr EOL
    | instr_list instr EOL
    ;

instr:
      HALT         { halt(); }
    | PUSH val     { push($2); }
    | POP reg      { pop($2); }
    | SET reg val  { set($2, $3); }

val:  reg
    | NUM
    ;

reg: REG           { $$ = 32768 + $1; }

%%

void yyerror(const char *s)
{
    printf("error: %s\n", s);
}

int main(void)
{
    yyparse();
}
