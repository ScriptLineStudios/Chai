#ifndef LEXER_H
#define LEXER_H

typedef struct {
	char *str;
} String;

String *lex_file(FILE *file_ptr);

#endif