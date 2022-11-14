#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "parser.h"
#include "lexer.h"

int pushes;
FILE *file_ptr;

FILE *get_file_ptr() {
    return file_ptr;
}

void codegen_setup(NodeReturn node) {
    file_ptr = fopen("code.asm", "w");

    fprintf(file_ptr, "global main\n");
    fprintf(file_ptr, "extern printf\n");
    fprintf(file_ptr, "section .text\n");
    fprintf(file_ptr, "main:\n");
    fprintf(file_ptr, "    sub rsp, 32\n");
}

int tok_markers = 0;

void codegen_number(NodeReturn node) {
    fprintf(file_ptr, ";; --- Number\n");
    Number *number = (Number *)node.node;
    fprintf(file_ptr, "    push %d\n", number->value);
    tok_markers++;
}

void codegen_add(NodeReturn node) {
    Number *number = (Number *)node.node;
    fprintf(file_ptr, "    pop rax\n");
    fprintf(file_ptr, "    pop rbx\n");
    fprintf(file_ptr, "    add rax, rbx\n");
    fprintf(file_ptr, "    push rax\n");
    tok_markers++;
}

void codegen_sub(NodeReturn node) {
    Number *number = (Number *)node.node;
    fprintf(file_ptr, "    pop rax\n");
    fprintf(file_ptr, "    pop rbx\n");
    fprintf(file_ptr, "    sub rbx, rax\n");
    fprintf(file_ptr, "    push rbx\n");
    tok_markers++;
}

void codegen_mult(NodeReturn node) {
    fprintf(file_ptr, "    pop rax\n");
    fprintf(file_ptr, "    pop rdx\n");
    fprintf(file_ptr, "    mul rdx\n");
    fprintf(file_ptr, "    push rax\n");
    tok_markers++;
}

int num_list_offset = -1;
void codegen_var_use(NodeReturn node) {
    UseVar *use_var = (UseVar *)node.node;
    fprintf(file_ptr, "    mov rax, [x+%d]\n", (use_var->index+num_list_offset) * 8);
    fprintf(file_ptr, "    push rax\n");
    tok_markers++;
}

void codegen_var(NodeReturn node) {
    fprintf(file_ptr, ";; --- Var\n");

    VarAssign *var = (VarAssign *)node.node;
    NodeReturn expr = var->expression;
    if (expr.node_type == LIST) {
        List *list = (List *)expr.node;
        for (int i = 0; i < list->size; i++) {
            fprintf(file_ptr, "    pop rax\n");
            fprintf(file_ptr, "    mov [x+%d], rax\n", (var->index+i) * 8); //push the value onto the stack
            num_list_offset++;
        }
    }
    else {
        fprintf(file_ptr, "    pop rax\n");
        fprintf(file_ptr, "    mov [x+%d], rax\n", (var->index+num_list_offset) * 8); //push the value onto the stack
    }

}


#define CMP_BUFFER 65525
char *cmp_buffer;

int cleanups = 0;

void codegen_string_not_equal(NodeReturn node) {
    BinOp *bin_op = (BinOp *)node.node;
    
    NodeReturn left = bin_op->left;
    NodeReturn right = bin_op->right;

    fprintf(file_ptr, "comp_%d:\n", bin_op->stack_pos); //Get the addresses of the 2 strings
    visit_node(left);
    visit_node(right);

    fprintf(file_ptr, "    pop rax\n"); //Get the addresses of the 2 strings
    fprintf(file_ptr, "    pop rbx\n"); //Get the addresses of the 2 strings

    fprintf(file_ptr, "    mov rsi, rax\n");
    fprintf(file_ptr, "    mov rdi, rbx\n");
    fprintf(file_ptr, "    mov ecx, 6\n");

    
    fprintf(file_ptr, "    repe cmpsb\n");
    fprintf(file_ptr, "    jne branch_%d\n", bin_op->stack_pos);
    fprintf(file_ptr, "    je end_branch_%d\n", bin_op->stack_pos);
    cleanups+=2;
}

#define MAX_TEMP_BUFFER_SIZE 128

void codegen_not_equal(NodeReturn node) {
    BinOp *bin_op = (BinOp *)node.node;

    NodeReturn left = bin_op->left;
    NodeReturn right = bin_op->right;

    fprintf(file_ptr, "comp_%d:\n", bin_op->stack_pos); //Get the addresses of the 2 strings
    visit_node(left);
    visit_node(right);

    fprintf(file_ptr, "    pop rax\n");
    fprintf(file_ptr, "    pop rbx\n");
    fprintf(file_ptr, "    cmp rax, rbx\n");    
    fprintf(file_ptr, "    jne branch_%d\n", bin_op->stack_pos);
    fprintf(file_ptr, "    je end_branch_%d\n", bin_op->stack_pos);
    cleanups += 2;
}

void codegen_while_node(NodeReturn node) {
    WhileNode *while_node = (WhileNode *)node.node;
    fprintf(file_ptr, "branch_%d:\n", while_node->stack_pos);
}

void codegen_if(NodeReturn node) {
    IfNode *if_node = (IfNode *)node.node;
    fprintf(file_ptr, "branch_%d:\n", if_node->stack_pos);
}


void codegen_end_node(NodeReturn node) {
    End *end_node = (End *)node.node;
    if (end_node->ending == IF) {
        fprintf(file_ptr, "end_branch_%d:\n", end_node->stack_pos);
    }
    else if (end_node->ending == WHILE) {
        fprintf(file_ptr, "    jmp comp_%d\n", end_node->stack_pos);
        fprintf(file_ptr, "end_branch_%d:\n", end_node->stack_pos);
    }
}

void codegen_stdout(NodeReturn node, NodeType type, char **var_types) {
    fprintf(file_ptr, ";; --- Stdout Start\n");

    fprintf(file_ptr, "    pop rax\n");
    if (type == NUMBER) {
        printf("here");
        fprintf(file_ptr, "    mov rdi, format\n");
    }
    else if (type == USEVAR) {
        UseVar *use_var = (UseVar *)node.node;
        if (var_types[getvariableindex(use_var->name)] == "string") {
            fprintf(file_ptr, "    mov rdi, string_format\n");
        }
        else {
            fprintf(file_ptr, "    mov rdi, format\n");
        }
    }
    else if (type == LISTACCESS) {
        fprintf(file_ptr, "    mov rdi, format\n");
    }
    else {
        fprintf(file_ptr, "    mov rdi, string_format\n");
    }
        
    fprintf(file_ptr, "    mov rsi, rax\n");
    fprintf(file_ptr, "    mov al,0\n");
    fprintf(file_ptr, "    xor rax, rax\n");
    fprintf(file_ptr, "    call printf\n");
}

char *strings[6400000] = {0}; 
int number_alloced_strings = 0;

void queue_string_data_gen(char *value) {
    strings[number_alloced_strings] = value;
    number_alloced_strings++;
}

void codegen_string(NodeReturn node) {
	String *string = (String *)node.node;
    queue_string_data_gen(string->value);
    fprintf(file_ptr, "    push string_%d\n", number_alloced_strings - 1);
}

void codegen_list(NodeReturn node) {
    List *list = (List *)node.node;

    for (int i = list->size - 1; i >= 0; i--) {
        fprintf(file_ptr, "    push %d\n", atoi(list->values[i]));
    }
}

void codegen_list_access(NodeReturn node) {
    fprintf(file_ptr, ";; --- List Access\n");
    ListAccess *list_access = (ListAccess *)node.node;
    fprintf(file_ptr, "    pop rbx\n");
    fprintf(file_ptr, "    mov rax, [x+%d+rbx*8]\n", (list_access->index) * 8);
    fprintf(file_ptr, "    push rax\n");
}

void codegen_list_reassign(NodeReturn node) {
    ListReassign *list_reassign = (ListReassign *)node.node;
    fprintf(file_ptr, ";; --- List Reassign\n");
    fprintf(file_ptr, "    pop rax\n"); // this is the value
    fprintf(file_ptr, "    pop rbx\n"); // this is the index
    fprintf(file_ptr, "    mov [x+%d+rbx*8], rax\n", (list_reassign->index) * 8);
}

void codegen_end() {
    for (int i = 0; i < cleanups; i++) {
        fprintf(file_ptr, "    pop rax\n");
    }
    fprintf(file_ptr, "    mov rax, 60\n");
    fprintf(file_ptr, "    syscall\n");

    fprintf(file_ptr, "    add rsp, 32\n");
    fprintf(file_ptr, "    ret\n");
    fprintf(file_ptr, "format:\n");
    fprintf(file_ptr, "    db %s, 10, 0\n", "\"%d\"");
    fprintf(file_ptr, "string_format:\n");
    fprintf(file_ptr, "    db %s, 10, 0\n", "\"%s\"");   
    
    for (int i = 0; i < number_alloced_strings; i++) {
        fprintf(file_ptr, "\nstring_%d:\n", i);
        //fprintf(file_ptr, "    db `%s`, %d, 0\n", strings[i], strlen(strings[number_alloced_strings - 1]));
        fprintf(file_ptr, "    db `%s`, %d, 0\n", strings[i], 9);
    
    }
    fprintf(file_ptr, "\nsection .data\n");
    fprintf(file_ptr, "    x db 0\n");
    fclose(file_ptr);
}
