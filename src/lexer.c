#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct {
	char *str;
} String;


static char *parse_identifier(char *line, int start, int *end, char *res) {
	int i = 0;

	for (i = start; i < strlen(line); i++) {
        if (isalnum(line[i])) {
			strncat(res, &line[i], 1);
        } else {
            break;
        }
	}

	*end = i;
	
	if (strcmp(res, "let") == 0) {
		return (char *)&res[0];
	}
	else {
		return "identifier";
	}
}

static char *parse_integer(char *line, int start, int *end, char *res) {
	int i = 0;

	for (i = start; i < strlen(line); i++) {
        if (isdigit(line[i])) {
			strncat(res, &line[i], 1);
        } else {
            break;
        }
	}

	*end = i;
	return (char *)&res[0];
}

String *lex_file(FILE *file_ptr) {
	char ch;
	char *str = malloc(sizeof(char) * 0);

	char *array = malloc(sizeof(char) * 0);
    char line[100];
	String *tokens = malloc(sizeof(String) * 100);
	int x = 0;

    while (fgets(line, 100, file_ptr)) {
		int skip = 1;
		line[strcspn(line, "\n")] = 0;
		for (int col = 0; col < strlen(line); col+=skip, skip=1) {
			String token;
			switch (line[col]) {
			case ' ':
			case '\t':
				break;
			case ';': 
				token.str = ";";
				tokens[x] = token;
				x++;
				break;
			case '=': 
				token.str = "=";
				tokens[x] = token;
				x++;
				break;
			case '(': 
				token.str = "(";
				tokens[x] = token;
				x++;
				break;
			case ')': 
				token.str = ")";
				tokens[x] = token;
				x++;
				break;
			case '+': 
				token.str = "+";
				tokens[x] = token;
				x++;
				break;
            case '*': 
				token.str = "*";
				tokens[x] = token;
				x++;
				break;
            case '/': 
				token.str = "/";
				tokens[x] = token;
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
			case '9': 
				char *_res = malloc(sizeof(char) * 100);
				token.str = parse_integer(line, col, &skip, _res);
				tokens[x] = token;
				x++;
				skip -= col;
				break;
			default: 
				char *res = malloc(sizeof(char) * 100);
				token.str = parse_identifier(line, col, &skip, res);
				tokens[x] = token;
				x++;
				skip -= col;
				break;
			}
		}

    }

    fclose(file_ptr);
	return tokens;
}