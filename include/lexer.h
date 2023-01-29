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
    TOK_OPEN_BRACKET = 9,
    TOK_CLOSE_BRACKET = 10,
    TOK_EQUAL_EQUAL = 11,
    TOK_OPEN_CURLY_BRACE = 12,
    TOK_CLOSE_CURLY_BRACE = 13,
    TOK_STRING = 14,
    TOK_BANG = 15, 
    TOK_BANG_EQUAL = 16,
} TokenType;

typedef struct {
    TokenType type;
    char *value;
    const char *filepath;
    int line_num;
    int position;
} Token;

typedef struct {
    const char *file_path;
    Token current_token;
    char *buffer;
    FILE *file;
    int file_offset;
    int line_num;
    int position;
} Lexer;

Lexer init_lexer(const char *file_path);
Token get_next_token(Lexer *lexer);

#endif