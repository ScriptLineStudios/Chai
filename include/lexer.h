#pragma once
#include <stdio.h>

typedef enum {
	TOK_UNKNOWN,
	TOK_IDENT,
	TOK_SEMI,
	TOK_LET,
	TOK_NUMBER,
	TOK_PLUS,
	TOK_MINUS,
	TOK_MULT,
	TOK_DIV,
	TOK_EQUALS,
	TOK_OPEN_PARENTHESES,
	TOK_CLOSE_PARENTHESES,
	TOK_NOT_EQUAL,
} TokenType;

typedef struct {
	TokenType type;
	char *value;
	int test;
} Token;

int lex_file(Token *tokens, FILE *file_ptr);
