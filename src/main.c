#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "../include/compiler.h"

void usage(void) {
	printf("USAGE: ./bin/comp.exe <filename> [EXTRA ARGS]\n");
	printf("[EXTRA ARGS]:\n");
	printf("    --link-file=your_c_file.c -> Link with C code.\n");
}

int main(int argc, char **argv) {
	if (argc > 1) {
		compile_file(argv[1]);
		return 0;
	}
	usage(); 
	return 1;
}

