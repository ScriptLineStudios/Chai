#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct {
	char *str;
} String;
int x = 0;

static char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

static char *parse_identifier(char *line, int start, int *end) {
	int i = 0;

	for (i = start; i < strlen(line); i++) {
        if (isalnum(line[i])) {
			printf("CHAR = %c\n", line[i]);
        } else {
			printf("ENDING...\n");
            break;
        }
	}

	*end = i;
	return "IDENTIFIER";
}


static char *parse_integer(char *line, int start, int *end) {
	int i = 0;

	for (i = start; i < strlen(line); i++) {
        if (isdigit(line[i])) {
            //result += c;
        } else {
            break;
        }
	}

	*end = i;
	return "INTEGER";
}

int main(int argc, char *argv[]) {
	FILE *file_ptr = fopen(argv[1], "r");
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
				token.str = parse_integer(line, col, &skip);
				tokens[x] = token;
				x++;
				skip -= col;
				break;
			default: 
				token.str = parse_identifier(line, col, &skip);
				tokens[x] = token;
				x++;
				skip -= col;
				break;
			}
		}

    }

    fclose(file_ptr);

	printf("\nTOKENS: \n");
	for (int x = 0; x < 40; x++) {
		printf("%s\n", tokens[x]);
	}
}
