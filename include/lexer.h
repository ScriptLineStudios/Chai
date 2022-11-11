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
	TOK_OPEN_CURLY_BRACE,
	TOK_CLOSE_CURLY_BRACE,
	TOK_STRING,
	TOK_OPEN_SQUARE_BRACKET,
	TOK_CLOSE_SQUARE_BRACKET,
	TOK_COMMA,
} TokenType;

typedef struct {
	TokenType type;
	char *value;
	int position;
	int line_num;
	const char *filepath;
} Token;

int lex_file(Token *tokens, FILE *file_ptr, const char *filepath);
