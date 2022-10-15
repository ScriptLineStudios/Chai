#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "../include/lexer.h"
#include "../include/parser.h"

int main(int argc, char *argv[]) {
	FILE *file_ptr = fopen(argv[1], "r");

	printf("STARTING FILE LEXING...\n");
	String *tokens = lex_file(file_ptr);
	printf("ENDING FILE LEXING...\n");

	// for (int x = 0; x < 40; x++) {
	// 	printf("%s\n", tokens[x]);
	// }
	generate_ast(tokens);

}
