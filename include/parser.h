#ifndef PARSER
#define PARSER

#include "lexer.h"

void parse_file(Lexer *lexer);

#define ADVANCE advance(lexer)
#define CAST(type, name) type *name = (type *)node.node;
#define TOK_MATCH(T, error) do{advance(lexer);if(current_token.type!=T){raise_compile_error(&current_token,error);}}while(false) //very readable
#define SEMI_MATCH do{if(current_token.type!=TOK_SEMI){raise_compile_error(&current_token,"Expected ;");advance(lexer);}}while(false) //very readable
#define STATEMENT_MATCH(name) strcmp(current_token.value, name) == 0
#define ALLOC_VAR(name) do{num_declared_vars++;variables=realloc(variables,sizeof(char*)*num_declared_vars);variables[num_declared_vars-1]=name;}while(false)

typedef enum {
    UNKNOWN = 0,
    NUMBER = 1,
    BINOP = 2, 
    VARDECAL = 3, 
    VARACCESS = 4,
    VARREASSIGN = 5,
} NodeType;

typedef struct {
    void *node;
    NodeType node_type;
} Node;

typedef struct {
    int value;
} Number;

typedef struct {
    Node left;
    TokenType op;
    Node right;
} BinOp;

typedef struct {
    char *var_name;
    Node expression;
} VarDecal;

typedef struct {
    char *var_name;
} VarAccess;

typedef struct {
    char *var_name;
    Node new_expression;
} VarReassign;

#endif