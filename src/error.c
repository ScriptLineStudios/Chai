#include <stdio.h>
#include <stdlib.h>

#include "../include/error.h"
#include "../include/lexer.h"
#include "../include/parser.h"

void raise_error(Token *token, const char *message) {
    printf("\033[31;1;4m%s:%d:%d ERROR:\033[0m", token->filepath, token->line_num, token->position);
    printf(" ");
    printf(message);
    printf("\n");
    exit(1);
}