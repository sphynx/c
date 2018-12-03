%{

#include <inttypes.h>
#include <stdio.h>

#include "codegen.h"
#include "table.h"

extern int yylex(void);
void yyerror(const char *s);

static int line_no = 1;

%}

%union {
    uint16_t num;
    struct label_entry *label;
}

%token EOL
%token <num> REG NUM CHAR
%token <num> HALT SET PUSH POP
%token <num> EQ GT JMP JT JF
%token <num> ADD MULT MOD AND OR
%token <num> NOT RMEM WMEM CALL
%token <num> RET OUT IN NOOP
%token <num> DW
%token <label> LABEL

%type <num> val reg out_arg address

%%

instr_list:
      opt_label_def opt_instr
    | instr_list opt_label_def opt_instr
    ;

opt_instr: eol
    | instr eol
    ;

opt_label_def:
      /* empty */
    | label_def
    ;

instr:
      HALT              { gen_code( 0U, 0); }
    | SET reg val       { gen_code( 1U, 2, $2, $3); }
    | PUSH val          { gen_code( 2U, 1, $2); }
    | POP reg           { gen_code( 3U, 1, $2); }
    | EQ reg val val    { gen_code( 4U, 3, $2, $3, $4); }
    | GT reg val val    { gen_code( 5U, 3, $2, $3, $4); }
    | JMP address       { gen_code( 6U, 1, $2); }
    | JT val address    { gen_code( 7U, 2, $2, $3); }
    | JF val address    { gen_code( 8U, 2, $2, $3); }
    | ADD reg val val   { gen_code( 9U, 3, $2, $3, $4); }
    | MULT reg val val  { gen_code(10U, 3, $2, $3, $4); }
    | MOD reg val val   { gen_code(11U, 3, $2, $3, $4); }
    | AND reg val val   { gen_code(12U, 3, $2, $3, $4); }
    | OR reg val val    { gen_code(13U, 3, $2, $3, $4); }
    | NOT reg val       { gen_code(14U, 2, $2, $3); }
    | RMEM reg address  { gen_code(15U, 2, $2, $3); }
    | WMEM address val  { gen_code(16U, 2, $2, $3); }
    | CALL address      { gen_code(17U, 1, $2); }
    | RET               { gen_code(18U, 0); }
    | OUT out_arg       { gen_code(19U, 1, $2); }
    | IN reg            { gen_code(20U, 1, $2); }
    | NOOP              { gen_code(21U, 0); }
    | DW NUM            { dw($2); }
    ;

label_def:
    LABEL ':'   { $1->address = current_addr(); $1->declared = 1; }

val:  reg
    | NUM
    ;

address:
      val
    | LABEL {
         if (!$1->declared)
            yyerror("Undeclared label");
         $$ = $1->address;
      }
    ;

reg: REG   { $$ = 32768 + $1; }
    ;

out_arg:
      val
    | CHAR
    ;

eol: EOL   { line_no++; }
   ;

%%

void yyerror(const char *s)
{
    fprintf(stderr, "line: %d, error: %s\n", line_no, s);
}

int main(void)
{
    init_label_table();
    yyparse();
    print_labels();
}
