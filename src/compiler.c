#include <stdio.h>

#include "../include/compiler.h"
#include "../include/lexer.h"
#include "../include/parser.h"

void compile_file(const char *file_path) {
    Lexer lexer = init_lexer(file_path);
    printf("%d\n", get_next_token(&lexer).type);
    printf("%d\n", get_next_token(&lexer).type);
    printf("%d\n", get_next_token(&lexer).type);
//    parse_file(lexer);
}