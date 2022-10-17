#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "lexer.h"
#include "parser.h"

int main(int argc, char *argv[]) {
	printf("STARTING FILE LEXING...\n");
	Token *tokens = malloc(sizeof(Token) * 100);
	int ntok = lex_file(tokens, fopen(argv[1], "r"));
	printf("ENDING FILE LEXING...\n");

	generate_ast(tokens, ntok);
}
