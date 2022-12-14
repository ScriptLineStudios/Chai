#include "lexer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_LINE_LEN 100

char *parse_identifier(char *line, int start, int *end, char *res) {
	int i = start;

	for (; i < strlen(line); i++) {
        if (isalnum(line[i]) || line[i] == '_') {
			strncat(res, &line[i], 1);
        } else {
            break;
        }
	}

	*end = i;
	return res;
}

char *parse_string(char *line, int start, int *end, char *res) {
	int i = start+1;
	for (; i < strlen(line); i++) {
        if (line[i] != '"') {
			strncat(res, &line[i], 1);
        } else {
        	i++;
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

int lex_file(Token *tokens, FILE *file_ptr, const char *filepath) {
    char line[MAX_LINE_LEN] = {0};
	memset(tokens, 0, sizeof(Token) * 100);
	int x = 0;
	int lines = 1;

    while (fgets(line, MAX_LINE_LEN, file_ptr)) {
		int skip = 1;
		line[strcspn(line, "\n")] = 0;
		for (int col = 0; col < strlen(line); col+=skip, skip=1) {
			tokens[x].position = x;
			tokens[x].line_num = lines;
			switch (line[col]) {
			case ' ':
			case '\t':
				break;
			case ';':
				tokens[x].type = TOK_SEMI;
				tokens[x].value = ";";
				x++;
				break;
			case '=': 
				if (line[col-1] != '!') {
					tokens[x].type = TOK_EQUALS;
					tokens[x].value = "=";
					x++;
					break;
				}
				break;
			case '(':
				tokens[x].type = TOK_OPEN_PARENTHESES;
				tokens[x].value = "(";
				x++;
				break;
			case ')': 
				tokens[x].type = TOK_CLOSE_PARENTHESES;
				tokens[x].value = ")";
				x++;
				break;
			case '[':
				tokens[x].type = TOK_OPEN_SQUARE_BRACKET;
				tokens[x].value = "[";
				x++;
				break;
			case ']': 
				tokens[x].type = TOK_CLOSE_SQUARE_BRACKET;
				tokens[x].value = "]";
				x++;
				break;
			case ',': 
				tokens[x].type = TOK_COMMA;
				tokens[x].value = ",";
				x++;
				break;
			case '+':
				tokens[x].type = TOK_PLUS;
				tokens[x].value = "+";
				x++;
				break;
			case '-':
				tokens[x].type = TOK_MINUS;
				tokens[x].value = "-";
				x++;
				break;
            case '*':
            	tokens[x].type = TOK_MULT;
				tokens[x].value = "*";
				x++;
				break;
            case '/': 
            	tokens[x].type = TOK_DIV;
				tokens[x].value = "/";
				x++;
				break;
			case '{': 
            	tokens[x].type = TOK_OPEN_CURLY_BRACE;
				tokens[x].value = "{";
				x++;
				break;
			case '}': 
            	tokens[x].type = TOK_CLOSE_CURLY_BRACE;
				tokens[x].value = "}";
				x++;
				break;
			case '!': 
				if (line[col+1] == '=') {
					tokens[x].type = TOK_NOT_EQUAL;
					tokens[x].value = "!=";
					x++;
					break;
				}
				break;
			case '"': 
				char *_res = malloc(sizeof(char) * 100);
				memset(_res, 0, 100);
				tokens[x].type = TOK_STRING;
				tokens[x].value = parse_string(line, col, &skip, _res);
				skip -= col;
				x++;
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
				tokens[x].type = TOK_NUMBER;
				tokens[x].value = parse_integer(line, col, &skip, _res);
				skip -= col;
				x++;
			}
			break;
			default: {
				char *res = malloc(sizeof(char) * 100);
				memset(res, 0, 100);
				tokens[x].type = TOK_IDENT;
				tokens[x].value = parse_identifier(line, col, &skip, res);
				skip -= col;
				x++;
			}
			break;
			}
			tokens[x].filepath = filepath;
		}
		lines++;
    }

    fclose(file_ptr);
	return x;
}
