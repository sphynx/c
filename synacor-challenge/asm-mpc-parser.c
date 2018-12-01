#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mpc.h"

int main(int argc, char *argv[])
{
    mpc_parser_t *Reg = mpc_new("reg");
    mpc_parser_t *Num = mpc_new("num");
    mpc_parser_t *Chr = mpc_new("chr");
    mpc_parser_t *Val = mpc_new("val");
    mpc_parser_t *Label = mpc_new("label");
    mpc_parser_t *Instr = mpc_new("instr");
    mpc_parser_t *Comment = mpc_new("comment");
    mpc_parser_t *Asmline = mpc_new("asmline");
    mpc_parser_t *Program = mpc_new("program");

    mpca_lang(MPCA_LANG_DEFAULT,
              " reg     : '%' /[0-7]/ ;"
              " num     : /[0-9]+/ ;"
              " chr     : /'.'/ ;"
              " val     : <reg> | <num> ;"
              " label   : /[a-zA-Z][a-zA-Z_0-9]*/ ':' ;"
              " instr   : \"halt\""
              "         | \"set\" <reg> <val>"
              "         | \"push\" <val>"
              "         | \"pop\" <reg>"
              "         | \"eq\" <reg> <val> <val>"
              "         | \"gt\" <reg> <val> <val>"
              "         | \"jmp\" <val>"
              "         | \"jt\" <val> <val>"
              "         | \"jf\" <val> <val>"
              "         | \"add\" <reg> <val> <val>"
              "         | \"mult\" <reg> <val> <val>"
              "         | \"mod\" <reg> <val> <val>"
              "         | \"and\" <reg> <val> <val>"
              "         | \"or\" <reg> <val> <val>"
              "         | \"not\" <reg> <val>"
              "         | \"rmem\" <reg> <val>"
              "         | \"wmem\" <val> <val>"
              "         | \"call\" <val>"
              "         | \"ret\""
              "         | \"out\" (<val> | <chr>)"
              "         | \"in\" <reg>"
              "         | \"noop\" ; "
              " comment : ';' /.*/ ;"
              " asmline : <label>* <instr> <comment>? ;"
              " program : /^/ <asmline>+ /$/;"
              ,
              Reg, Num, Chr, Val, Label, Instr, Comment, Asmline, Program, NULL);

    mpc_result_t r;

    if (argc > 1) {
        FILE *f = fopen(argv[1], "rb");
        if (f == NULL) {
            perror(argv[1]);
            exit(EXIT_FAILURE);
        }

        if (mpc_parse_file(argv[1], f, Program, &r)) {
            mpc_ast_print(r.output);
            mpc_ast_delete(r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }
        fclose(f);
    }

    mpc_cleanup(9, Reg, Num, Chr, Val, Label, Instr, Comment, Asmline, Program);

    return 0;
}
