#ifndef LEXER
#define LEXER

typedef enum {
    TOK_NONE = 0,
    TOK_IDENTIFIER = 1,
    TOK_ADD = 2,
    TOK_SUB = 3,
    TOK_DIV = 4,
    TOK_MULT = 5,
    TOK_NUMBER = 6,
    TOK_SEMI = 7,
    TOK_EQUAL = 8,
} TokenType;

typedef struct {
    TokenType type;
    char *value;
} Token;

typedef struct {
    const char *file_path;
    Token current_token;
    char *buffer;
    FILE *file;
    int file_offset;
} Lexer;

Lexer init_lexer(const char *file_path);
Token get_next_token(Lexer *lexer);

#endif