#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "../include/lexer.h"

Lexer init_lexer(const char *file_path) {
    Lexer lexer;
    lexer.file_offset = 0;
    lexer.file_path = file_path;
    lexer.line_num = 1;
    lexer.position = 0;

    lexer.file = fopen(file_path, "rb");

    fseek(lexer.file, 0L, SEEK_END);
    size_t file_size = ftell(lexer.file);
    rewind(lexer.file);

    lexer.buffer = (char *)malloc(file_size + 1);
    size_t bytes_read = fread(lexer.buffer, sizeof(char), file_size, lexer.file);

    fclose(lexer.file);

    return lexer;
}

char *concat(char *dest, char src, int len) {
    dest[len-1] = src;
    dest = realloc(dest, sizeof(char *) * len);
    return dest;
}

Token parse_identifier(Lexer *lexer) {
    char current_char = lexer->buffer[lexer->file_offset];
    int len = 1;
    char *value = malloc(sizeof(char) * len);
    while (isalnum(current_char)) {
        value = concat(value, current_char, len);
        len++;
        lexer->file_offset++;
        current_char = lexer->buffer[lexer->file_offset];
    }
    value[len-1] = '\0';
    Token ident_token;
    ident_token.type = TOK_IDENTIFIER;
    ident_token.value = value;
    return ident_token;
}

Token parse_number(Lexer *lexer) {
    char current_char = lexer->buffer[lexer->file_offset];
    int len = 1;
    char *value = malloc(sizeof(char) * len);
    while (isdigit(current_char)) {
        value = concat(value, current_char, len);
        len++;
        lexer->file_offset++;
        current_char = lexer->buffer[lexer->file_offset];
    }
    value[len] = '\0';
    Token number_token;
    number_token.type = TOK_NUMBER;
    number_token.value = value;
    return number_token;
}

Token get_next_token(Lexer *lexer) {
    Token token;
    token.filepath = lexer->file_path;
    token.position = 0;
    token.line_num = lexer->line_num;

    char current_char = lexer->buffer[lexer->file_offset];

    while (current_char==' '||current_char=='\n') {
        lexer->file_offset++;
        current_char = lexer->buffer[lexer->file_offset];
    }

    if (isdigit(current_char)) {
        return parse_number(lexer);
    }

    switch (current_char) {
        case '+':
            token.type = TOK_ADD;
            break;
        case '-':
            token.type = TOK_SUB;
            break;
        case '/':
            token.type = TOK_DIV;
            break;
        case '*':
            token.type = TOK_MULT;
            break;
        case ';': 
            token.type = TOK_SEMI;
            break;
        case '=': 
            token.type = TOK_EQUAL;
            break;
        default:
            return parse_identifier(lexer);
    }

    lexer->file_offset++;
    return token;
}

// Token get_next_token(Lexer *lexer) {
//     Token token;
//     token.filepath = lexer->file_path;
//     token.position = 0;
//     token.line_num = lexer->line_num;

//     char current_char = lexer->buffer[lexer->file_offset];
    
//     while (current_char == ' ' || current_char == '\n') {
//         current_char = lexer->buffer[lexer->file_offset];
//         lexer->file_offset++;
//     }

//     printf("looking at: %c %d\n", current_char, lexer->file_offset);
//     if isdigit(current_char) {
//         return parse_number(lexer);
//     }

//     switch (current_char) {
//         case '\0':
//             break;
//         case '+':
//             token.type = TOK_ADD;
//             break;
//         case '-':
//             token.type = TOK_SUB;
//             break;
//         case '/':
//             token.type = TOK_DIV;
//             break;
//         case '*':
//             token.type = TOK_MULT;
//             break;
//         case ';': 
//             token.type = TOK_SEMI;
//             break;
//         case '=': 
//             token.type = TOK_EQUAL;
//             break;
//         default:
//             //printf("token does not match! looking for identifier\n");
//             return parse_identifier(lexer);
//     }

//     lexer->file_offset++;
//     return token;
// }

