#include <stdio.h>

#include "../include/compiler.h"
#include "../include/lexer.h"
#include "../include/parser.h"

void compile_file(const char *file_path) {
    Lexer lexer = init_lexer(file_path);
    parse_file(&lexer);
}