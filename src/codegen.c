#include <stdio.h>
#include <stdlib.h>
#include "../include/codegen.h"
#include "../include/parser.h"
#include "../SSCB/include/sscb_codegen.h"
#include "../SSCB/include/sscb_instructions.h"
#include "../SSCB/include/sscb_optimizer.h"
#include "../SSCB/include/sscb_virtual_machine.h"

void codegen_init() {
    instruction_setup();
}

void codegen_end() {
    optimise_generated_instructions(1);
    codegen_generated_instructions(INTEL_x86_64_LINUX);
}

void codegen_number(Node node) {
    CAST(Number, number);
    PUSH(IMM(number->value));
}

void codegen_binop(Node node) {
    CAST(BinOp, bin_op);
    
    TokenType op = bin_op->op;

    switch (op) {
        case TOK_ADD:
            POP(REG(R1));
            POP(REG(R2));
            ADD(REG(R1), REG(R2));
            break;
        case TOK_DIV:
            POP(REG(R1));
            POP(REG(R2));
            DIV(REG(R1), REG(R2));
            break;
        case TOK_MULT:
            POP(REG(R1));
            POP(REG(R2));
            MUL(REG(R2));
            break;
        case TOK_SUB:
            POP(REG(R1));
            POP(REG(R2));
            SUB(REG(R1), REG(R2));
            break;
    }
}

