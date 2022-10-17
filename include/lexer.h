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
} TokenType;

typedef struct {
	TokenType type;
	char *value;
} Token;

Token *lex_file(FILE *file_ptr);
