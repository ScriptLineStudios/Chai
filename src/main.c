#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "lexer.h"
#include "parser.h"

#include "run.c"

void usage(void) {
	printf("USAGE: ./bin/comp.exe <filename>\n");
}

void run(void) {
	system("nasm -f elf64 code.asm");
	system("gcc -no-pie code.o");
	system("./a.out");
	system("rm code.o && rm code.asm");
}

int main(int argc, char *argv[]) {
	if (argc > 1) {
		Token *tokens = malloc(sizeof(Token) * 100);
		int ntok = lex_file(tokens, fopen(argv[1], "r"), argv[1]);

		generate_ast(tokens, ntok);
		free(tokens);

		run();

		return 0;
	}
	usage();
	return 1;
}

