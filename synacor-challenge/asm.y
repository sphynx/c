%{

#include <inttypes.h>
#include <stdio.h>

#include "codegen.h"

extern int yylex(void);
void yyerror(const char *s);

static int line_no = 1;

%}

%define api.value.type {uint16_t}

%token EOL
%token REG NUM CHAR
%token HALT SET PUSH POP
%token EQ GT JMP JT JF
%token ADD MULT MOD AND OR
%token NOT RMEM WMEM CALL
%token RET OUT IN NOOP
%token DW

%%

instr_list: instr eol
    | instr_list instr eol
    ;

instr:
      HALT              { gen_code( 0U, 0); }
    | SET reg val       { gen_code( 1U, 2, $2, $3); }
    | PUSH val          { gen_code( 2U, 1, $2); }
    | POP reg           { gen_code( 3U, 1, $2); }
    | EQ reg val val    { gen_code( 4U, 3, $2, $3, $4); }
    | GT reg val val    { gen_code( 5U, 3, $2, $3, $4); }
    | JMP val           { gen_code( 6U, 1, $2); }
    | JT val val        { gen_code( 7U, 2, $2, $3); }
    | JF val val        { gen_code( 8U, 2, $2, $3); }
    | ADD reg val val   { gen_code( 9U, 3, $2, $3, $4); }
    | MULT reg val val  { gen_code(10U, 3, $2, $3, $4); }
    | MOD reg val val   { gen_code(11U, 3, $2, $3, $4); }
    | AND reg val val   { gen_code(12U, 3, $2, $3, $4); }
    | OR reg val val    { gen_code(13U, 3, $2, $3, $4); }
    | NOT reg val       { gen_code(14U, 2, $2, $3); }
    | RMEM reg val      { gen_code(15U, 2, $2, $3); }
    | WMEM val val      { gen_code(16U, 2, $2, $3); }
    | CALL val          { gen_code(17U, 1, $2); }
    | RET               { gen_code(18U, 0); }
    | OUT out_arg       { gen_code(19U, 1, $2); }
    | IN reg            { gen_code(20U, 1, $2); }
    | NOOP              { gen_code(21U, 0); }
    | DW NUM            { dw($1); }
    ;

val:  reg
    | NUM
    ;

reg: REG           { $$ = 32768 + $1; }
   ;

out_arg: val
    | CHAR
    ;

eol: EOL { line_no++; }
   ;

%%

void yyerror(const char *s)
{
    fprintf(stderr, "line: %d, error: %s\n", line_no, s);
}

int main(void)
{
    return yyparse();
}
