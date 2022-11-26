#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "lexer.h"
#include "parser.h"

#include "run.c"

#include "../SSCB/src/sscb_codegen.c"
#include "../SSCB/src/sscb_instructions.c"
#include "../SSCB/src/sscb_optimizer.c"

void usage(void) {
	printf("USAGE: ./bin/comp.exe <filename> [EXTRA ARGS]\n");
	printf("[EXTRA ARGS]:\n");
	printf("    --link-file=your_c_file.c -> Link with C code.\n");
}

char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

char *link_arg = "gcc -no-pie code.o";

void run(void) {
	system("nasm -f elf64 code.asm");
	system(link_arg);
	system("./a.out");
	//system("rm code.o && rm code.asm && rm a.out");
}

char *get_name_from_file_path(char *filename) {
	return strtok(strrchr(filename, '/'), "/");
}

void link_file(char *file_path) {
	char command[1000];
	// sprintf(command, "gcc -shared -o %s.so %s.c", get_name_from_file_path(file_path), strtok(file_path, "."));
	// system(command);
	// sprintf(command, "LD_LIBRARY_PATH=$LD_LIBRARY_PATH:%s.so", get_name_from_file_path(file_path));
	// system(command);
	// system("export LD_LIBRARY_PATH");
	// sprintf(command, " -Xlinker %s.so ", get_name_from_file_path(file_path));
	sprintf(command, "gcc -c -o %s.o %s.c", get_name_from_file_path(file_path), strtok(file_path, "."));
	system(command);
	sprintf(command, " -static %s.o", get_name_from_file_path(file_path));
	link_arg = concat(link_arg, command);
}

void process_addition_arg(char *addition_arg) {
	if (strncmp(addition_arg, "--", 2) == 0) {
		int scanner = 2;
		char *new_str = malloc(sizeof(char) * 100);
		char *operand = malloc(sizeof(char) * 100);
		
		while (addition_arg[scanner] != '=') {
			new_str[scanner - 2] = addition_arg[scanner];
			scanner++;
		}
		scanner++;
		int new_scan = scanner;
		while (addition_arg[new_scan] != '\0') {
			operand[new_scan - scanner] = addition_arg[new_scan];
			new_scan++;
		}

		new_str[scanner - 3] = '\0';
	
		if (strcmp(new_str, "link-file") == 0) {
			link_file(operand);
		}
	}
}

int main(int argc, char *argv[]) {
	if (argc > 1) {
		Token *tokens = malloc(sizeof(Token) * 100);
		int ntok = lex_file(tokens, fopen(argv[1], "r"), argv[1]);
		for (int i = 2; i < argc; i++) {
			process_addition_arg(argv[i]);
		}
 
		generate_ast(tokens, ntok);
		free(tokens);

		run();

		return 0;
	}
	usage();
	return 1;
}

