%{
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "codegen.h"
#include "table.h"

extern int yylex(void);
extern void yyrestart(FILE*);
extern FILE *yyin;

void yyerror(const char *s);

static int line_no = 1;
static FILE *out = NULL;

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
      HALT              { gen_code(out,  0U, 0); }
    | SET reg val       { gen_code(out,  1U, 2, $2, $3); }
    | PUSH val          { gen_code(out,  2U, 1, $2); }
    | POP reg           { gen_code(out,  3U, 1, $2); }
    | EQ reg val val    { gen_code(out,  4U, 3, $2, $3, $4); }
    | GT reg val val    { gen_code(out,  5U, 3, $2, $3, $4); }
    | JMP address       { gen_code(out,  6U, 1, $2); }
    | JT val address    { gen_code(out,  7U, 2, $2, $3); }
    | JF val address    { gen_code(out,  8U, 2, $2, $3); }
    | ADD reg val val   { gen_code(out,  9U, 3, $2, $3, $4); }
    | MULT reg val val  { gen_code(out, 10U, 3, $2, $3, $4); }
    | MOD reg val val   { gen_code(out, 11U, 3, $2, $3, $4); }
    | AND reg val val   { gen_code(out, 12U, 3, $2, $3, $4); }
    | OR reg val val    { gen_code(out, 13U, 3, $2, $3, $4); }
    | NOT reg val       { gen_code(out, 14U, 2, $2, $3); }
    | RMEM reg address  { gen_code(out, 15U, 2, $2, $3); }
    | WMEM address val  { gen_code(out, 16U, 2, $2, $3); }
    | CALL address      { gen_code(out, 17U, 1, $2); }
    | RET               { gen_code(out, 18U, 0); }
    | OUT out_arg       { gen_code(out, 19U, 1, $2); }
    | IN reg            { gen_code(out, 20U, 1, $2); }
    | NOOP              { gen_code(out, 21U, 0); }
    | DW NUM            { dw(out, $2); }
    ;

label_def:
    LABEL ':'   { $1->address = current_addr(); $1->declared = 1; }

val:  reg
    | NUM
    ;

address:
      val
    | LABEL { if ($1->declared) { $$ = $1->address; } }
    | '$'   { $$ = current_addr(); }
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

int main(int argc, char *argv[])
{
    if (argc != 3) {
       fprintf(stderr, "usage: %s <asm file> <out file>\n", argv[0]);
       exit(EXIT_FAILURE);
    }

    yyin = fopen(argv[1], "rb");
    if (yyin == NULL) {
       perror(argv[1]);
       exit(EXIT_FAILURE);
    }

    init_label_table();
    yyparse();

    // Reset and parse again, this time generating code.
    line_no = 1;
    out = fopen(argv[2], "wb");
    if (out == NULL) {
       perror(argv[2]);
       exit(EXIT_FAILURE);
    }

    yyin = fopen(argv[1], "rb");
    if (yyin == NULL) {
       perror(argv[1]);
       exit(EXIT_FAILURE);
    }
    yyrestart(yyin);
    gen_reset();
    yyparse();
    print_labels();
}
