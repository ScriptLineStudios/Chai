#ifndef ERROR
#define ERROR

#include "lexer.h"

void raise_compile_error(Token *token, const char *message);

#endif