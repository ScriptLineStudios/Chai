#include "lexer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_LINE_LEN 100

char *parse_identifier(char *line, int start, int *end, char *res) {
	int i = start;

	for (; i < strlen(line); i++) {
        if (isalnum(line[i])) {
			strncat(res, &line[i], 1);
        } else {
            break;
        }
	}

	*end = i;
	return res;
}

char *parse_integer(char *line, int start, int *end, char *res) {
	int i = start;

	for (; i < strlen(line); i++) {
        if (isdigit(line[i])) {
			strncat(res, &line[i], 1);
        } else {
            break;
        }
	}

	*end = i;
	return res;
}

Token *lex_file(FILE *file_ptr) {
    char line[MAX_LINE_LEN] = {0};
	Token *tokens = malloc(sizeof(Token) * 100);
	memset(tokens, 0, sizeof(Token) * 100);
	int x = 0;

    while (fgets(line, MAX_LINE_LEN, file_ptr)) {
    	printf("%i\n", tokens[0].type);
		int skip = 1;
		line[strcspn(line, "\n")] = 0;
		for (int col = 0; col < MAX_LINE_LEN; col+=skip, skip=1) {
			switch (line[col]) {
			case ' ':
			case '\t':
				break;
			case ';':
				tokens[x++].type = TOK_SEMI;
				break;
			case '=': 
				tokens[x++].type = TOK_EQUALS;
				break;
			case '(':
				tokens[x++].type = TOK_OPEN_PARENTHESES;
				break;
			case ')': 
				tokens[x++].type = TOK_CLOSE_PARENTHESES;
				break;
			case '+':
				tokens[x++].type = TOK_PLUS;
				break;
            case '*':
            	tokens[x++].type = TOK_MINUS;
				break;
            case '/': 
            	tokens[x++].type = TOK_DIV;
				break;
			case '0':        
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9': {
				char *_res = malloc(sizeof(char) * 100);
				memset(_res, 0, 100);
				tokens[x++].type = TOK_NUMBER;
				tokens[x].value = strdup(parse_integer(line, col, &skip, _res));
				skip -= col;
			}
			break;
			default: {
				char *res = malloc(sizeof(char) * 100);
				memset(res, 0, 100);
				tokens[x++].type = TOK_IDENT;
				tokens[x].value = strdup(parse_identifier(line, col, &skip, res));
				skip -= col;
			}
			break;
			}
		}

    }

    fclose(file_ptr);
	return tokens;
}
