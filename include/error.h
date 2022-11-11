#ifndef ERROR_H
#define ERROR_H

#include "../include/parser.h"
#include "../include/lexer.h"

void raise_error(Token *token, const char *message);

#endif