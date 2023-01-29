#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "../include/codegen.h"
#include "../include/parser.h"
#include "../SSCB/include/sscb_codegen.h"
#include "../SSCB/include/sscb_instructions.h"
#include "../SSCB/include/sscb_optimizer.h"
#include "../SSCB/include/sscb_virtual_machine.h"

static int number_strings;
char **strings;

void codegen_init() {
    strings = malloc(sizeof(char*) * number_strings);
    instruction_setup();
    FUNCTIONEXTERN(FUNCTION("printf"));
    FUNCTIONEXTERN(FUNCTION("exit"));
    PUSH(IMM(0));
}

void codegen_end() {
    CALL(FUNCTION("exit"));
    LABELDEF(LABEL("format"));
    DEFINEBYTE(STRING("%s, 10, 0", "\"%d\""));
    LABELDEF(LABEL("str_format"));
    DEFINEBYTE(STRING("%s, 10, 0", "\"%s\""));

    DEFINEBYTE(STRING("section .data", 0));
    for (int i = 0; i < number_strings; i++) {
        // DEFINEBYTE(STRING("string_%d: \"%s\"", i, strings[i]));
        LABELDEF(STRING("string_%d", i));   
        DEFINEBYTE(STRING("\"%s\", 0", strings[i]));   

    }  
    LABELDEF(LABEL("variables"));
    DEFINEBYTE(STRING("0", 0));
    optimise_generated_instructions(1);
    codegen_generated_instructions(INTEL_x86_64_LINUX);
}

static int if_index;

void codegen_set_parsing_if(int index) {
    if_index = index;
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
            SUB(REG(R2), REG(R1));
            break;
        case TOK_EQUAL_EQUAL:
// cmp rbx, rax, 
// pushf
// pop rax,
// or rax, 0x0040
// sub rax, 581

// cmp rax, 1
// je end
// mov rax, 0

            POP(REG(R1));
            POP(REG(R2));
            CMP(REG(R2), REG(R1));
            PUSH_FLAGS();
            POP(REG(R1));
            OR(REG(R1), IMM(64));
            SUB(REG(R1), IMM(581));
            CMP(REG(R1), IMM(1));
            JE(LABEL("end"));
            MOV(REG(R1), IMM(0));
            LABELDEF(LABEL("end"));
            PUSH(REG(R1));
            break;

    }
    if (op != TOK_EQUAL_EQUAL) {
        if (op != TOK_SUB) {
            PUSH(REG(R1));
        }
        else {
            PUSH(REG(R2));
        }
    }
}

void codegen_if(Node node) {
    CAST(IfNode, ifnode);
    LABELDEF(STRING("jmp_%d", ifnode->index));
}

void codegen_var_decal(Node node) {
    CAST(VarDecal, var_decal);
    POP(REG(R1));
    MOV(MEM("variables", (var_decal->index)*8), REG(R1));
}

void queue_string_gen(char *string) {
    number_strings++;
    strings = realloc(strings, sizeof(char*) * number_strings);
    strings[number_strings - 1] = string;
}   

void codegen_string(Node node) {
    CAST(String, string);
    PUSH(STRING("string_%d", string->index));
}

void codegen_print(Node node) {
    CAST(Print, print);
    POP(REG(R1));
    switch (print->expression.node_type) {
        case STRING:
            MOV(REG(ARGREGISTER1), LABEL("str_format"));
            break;
        case VARACCESS:
            int type = (int)((VarAccess*)print->expression.node)->type;
            printf("here %d\n", type);
            if (type == 1) {
                MOV(REG(ARGREGISTER1), LABEL("str_format"));
            }
            else {
                printf("%d\n", type);
                assert(false);
            }
            break;
        default:   
            printf("%d\n", print->expression.node_type);
            MOV(REG(ARGREGISTER1), LABEL("format"));
            break;
    }
    MOV(REG(ARGREGISTER2), REG(R1));
    XOR(REG(R1), REG(R1));
    CALL(FUNCTION("printf"));
}

void codegen_var_access(Node node) {
    CAST(VarAccess, var_access);
    MOV(REG(R1), MEM("variables", (var_access->index-1)*8));
    PUSH(REG(R1));
}

int num_unaries = -1;

void codegen_unary(Node node) {
    num_unaries++;
    CAST(Unary, unary);
    switch (unary->type) {
        case TOK_BANG:
            POP(REG(R1));
            CMP(REG(R1), IMM(1));
            JE(STRING("invert_1_%d", num_unaries));
            JNE(STRING("invert_0_%d", num_unaries));

            LABELDEF(STRING("invert_0_%d", num_unaries));
            MOV(REG(R1), IMM(1));
            JMP(STRING("end_%d", num_unaries));

            LABELDEF(STRING("invert_1_%d", num_unaries));
            MOV(REG(R1), IMM(0));

            LABELDEF(STRING("end_%d", num_unaries));
            PUSH(REG(R1));
            break;
        case TOK_SUB:
            POP(REG(R1));
            MOV(REG(R2), IMM(0));
            SUB(REG(R2), REG(R1));
            PUSH(REG(R2));
    }
}

codegen_var_reassign(Node node) {
    CAST(VarReassign, var_reassign);

    POP(REG(R1));
    MOV(MEM("variables", (var_reassign->index)*8 - 8), REG(R1));
}   
