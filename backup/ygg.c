#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

enum TokenType {
	NUM,
	ADD_OPE,
	SUB_OPE,
	MUL_OPE,
	DIV_OPE,
	IF,
	ELSE,
	BRACKET_START,
	BRACKET_END,
	BLOCK_START,
	BLOCK_END,
	PRINT,
	END
};

struct Token {
	enum TokenType type;
	void* data;
};

struct Node {
	void *data;
	struct Node *left;
	struct Node *right;
};

struct Token *tokens[100];
int tokensLength;
int pos;

struct Token *NewToken(enum TokenType type, void* data) {
	struct Token *tokenTmp;
	tokenTmp = (struct Token *)malloc(sizeof(struct Token));
	tokenTmp->type = type;
	tokenTmp->data = data;
	return tokenTmp;
}

void tokenize(char *sArg) {
	tokensLength = 0;
	char *s;

	s = sArg;
	while (*s != '\0') {
		if (isdigit(*s)) {
			tokens[tokensLength] = NewToken(NUM, (void *)malloc(sizeof(int)));
			*(int *)(tokens[tokensLength]->data) = strtol(s, &s, 10);
			tokensLength++;
		}

		if (strncmp(s, "if", 2) == 0) {
			if (!isalnum(*( s + 2 ))) {
				tokens[tokensLength] = NewToken(IF, NULL);
				tokensLength++;
				s += 2;
				continue;
			}
		}

		if (strncmp(s, "else", 4) == 0) {
			if (!isalnum(*( s + 4 ))) {
				tokens[tokensLength] = NewToken(ELSE, NULL);
				tokensLength++;
				s += 4;
				continue;
			}
		}

		if (strncmp(s, "print", 5) == 0) {
			if (!isalnum(*( s + 5 ))) {
				tokens[tokensLength] = NewToken(PRINT, NULL);
				tokensLength++;
				s += 5;
				continue;
			}
		}

		if (isalpha(*s)) {

		}

		if (*s == '{') {
			tokens[tokensLength] = NewToken(BLOCK_START, NULL);
			tokensLength++;
		}

		if (*s == '}') {
			tokens[tokensLength] = NewToken(BLOCK_END, NULL);
			tokensLength++;
		}

		if (*s == '(') {
			tokens[tokensLength] = NewToken(BRACKET_START, NULL);
			tokensLength++;
		}

		if (*s == ')') {
			tokens[tokensLength] = NewToken(BRACKET_END, NULL);
			tokensLength++;
		}

		if (*s == '+') {
			tokens[tokensLength] = NewToken(ADD_OPE, NULL);
			tokensLength++;
		}

		if (*s == '-') {
			tokens[tokensLength] = NewToken(SUB_OPE, NULL);
			tokensLength++;
		}

		if (*s == '*') {
			tokens[tokensLength] = NewToken(MUL_OPE, NULL);
			tokensLength++;
		}

		if (*s == '/') {
			tokens[tokensLength] = NewToken(DIV_OPE, NULL);
			tokensLength++;
		}

		if (*s == ';') {
			tokens[tokensLength] = NewToken(END, NULL);
			tokensLength++;
		}

		s++;
	}
}

struct Node *NewNode(void* data, struct Node *left, struct Node *right) {
	struct Node *node;
	node = (struct Node *)malloc(sizeof(struct Node));
	node->data = data;
	node->left = left;
	node->right = right;
	return node;
}

struct Token *getToken(struct Node *node) {
	return (struct Token *)node->data;
}

void printToken(struct Token *token) {
	if (token->type == NUM) printf("NUM : %d", *(int *)(token->data));
	if (token->type == ADD_OPE) printf("ADD_OPE");
	if (token->type == MUL_OPE) printf("MUL_OPE");
	if (token->type == IF) printf("IF");
	if (token->type == ELSE) printf("ELSE");
	if (token->type == BRACKET_START) printf("(");
	if (token->type == BRACKET_END) printf(")");
	if (token->type == BLOCK_START) printf("{");
	if (token->type == BLOCK_END) printf("}");
	if (token->type == PRINT) printf("PRINT");
	if (token->type == END) printf(";");
	printf("\n");
}

void printTokens() {
	int i;
	for (i = 0; i < tokensLength; i++) {
		printf("%2d: ", i);
		printToken(tokens[i]);
	}
}

int consume(enum TokenType type) {
	// pos < tokensLength なくてもイケる？
	if (pos < tokensLength && tokens[pos]->type == type) {
		return 1;
	}
	return 0;
}

struct Node *term() {
	struct Token *token;
	
	token = tokens[pos];
	// printToken(token);

	if (consume(NUM)) {
		pos++;
		return NewNode((void *)token, NULL, NULL);
	} else {
		token = tokens[pos];
		printf("syntax error : Not term.\n");
		return NULL;
	}
}

struct Node *mul() {
	struct Node *node = term();
	struct Token *token;

	while (1) {
		if (consume(MUL_OPE)) {
			token = tokens[pos];
			pos++;
			node = NewNode((void *)token, node, term());
		} else {
			return node;
		}
	}
}

struct Node *expr() {
	struct Node *node;
	struct Token *token;

	if (consume(PRINT)) {
		token = tokens[pos];
		pos++;
		return NewNode((void *)token, NULL, expr());
	}

	node = mul();

	while (1) {
		if (consume(ADD_OPE)) {
			token = tokens[pos];
			pos++;
			node = NewNode((void *)token, node, mul());
		} else {
			return node;
		}
	}
}

struct Node *calc(struct Node *node) {
	enum TokenType type = ((struct Token *)node->data)->type;
	struct Node *lResult;
	struct Node *rResult;
	struct Node *ret;
	if (type == NUM) {
		// return *(int *)((struct Token *)node->data)->data;
		return NewNode(node->data, NULL, NULL);
	} else {
		if (type == PRINT) {
			ret = calc(node->right);
			if (getToken(ret)->type == NUM) {
				printf("%d", *(int *)getToken(ret)->data);
			}
			// printf("hogehogehoge\n");
			return NULL;
		}
		lResult = calc(node->left);
		rResult = calc(node->right);
		if (type == MUL_OPE) {
			*(int *)getToken(lResult)->data *= *(int *)getToken(rResult)->data;
			return lResult;
		} else if (type == ADD_OPE) {
			*(int *)getToken(lResult)->data += *(int *)getToken(rResult)->data;
			return lResult;
		} else {
			return NULL;
		}
	}
}

void skipBlock() {
	if (!consume(BLOCK_START)) {
		return;
	}

	while (!consume(BLOCK_END)) {
		if (consume(BLOCK_START)) {
			skipBlock();
		}
		pos++;
	}
	pos++;
}

struct Node *stmt() {
	struct Node *node;
	struct Token *token;
	struct Node *ret;

	// if (consume(PRINT)) {
	// 	pos++;
	// 	ret = calc(expr());
	// 	if (((struct Token *)ret->data)->type == NUM) {
	// 		printf("NUM : %d\n", *(int *)((struct Token *)ret->data)->data);
	// 	}
	// 	return NULL;
	// }

	if (consume(BLOCK_START)) {
		pos++;
		while (!consume(BLOCK_END)) {
			node = stmt();
			if (node != NULL) {
				calc(node);
			}
		}
		pos++;
		return NULL;
	}

	if (consume(IF)) {
		token = tokens[pos];
		pos++;

		if (!consume(BRACKET_START)) {
			printf("syntax error : Write a bracket after if.\n");
			exit(-1);
		}

		pos++;
		node = expr();

		if (consume(BRACKET_END)) {
			pos++;
		}

		ret = calc(node);

		node = stmt();

		if (*(int *)getToken(ret)->data != 0) {
			// else節を読み飛ばす
			if (consume(ELSE)) {
				pos++;
				if (consume(BLOCK_START)) {
					skipBlock();
					printToken(tokens[pos]);
				} else {
					// printToken(tokens[pos]);
					stmt();
				}
			}
			return node;
		} else {
			if (consume(ELSE)) {
				pos++;
				return stmt();
			}
		}
		return NULL;
	}

	node = expr();

	if (consume(END)) {
		pos++;
		return node;
	}

	printf("syntax error : Unknown syntax.\n");

	return NULL;
}

int main(int argv, char *args[]) {
	FILE *fp;
	char *fname;
	char psBuf[256][256];
	char s[4096];
	int iSize;
	int i;
	struct Node *node;

	if (argv != 2) {
		printf("Illegal argument error.\n");
		exit(1);
	}

	fname = args[1];

	fp = fopen(fname, "r");
	if (fp == NULL) {
		return -1;
	}

	iSize = 0;
	while (fscanf(fp, "%s\n", psBuf[iSize]) != EOF) {
	// while (fgets(psBuf[iSize], 256, fp) != NULL) {
		iSize++;
	}

	for (i = 0; i < iSize; i++) {
		strcat(s, psBuf[i]);
		strcat(s, " ");
	}

	// printf("%s\n", s);

	tokenize(s);
	printTokens();

	printf("----------\n");

	pos = 0;

	// expr();

	// printf("%d\n", ((struct Token *)e->left->data)->type);
	// printf("%d\n", *(int *)((struct Token *)e->left->data)->data);

	// printf("%d\n", calc(e));

	// i = 0;
	// while (pos < tokensLength) {
	// 	node = stmt();
	// 	if (node != NULL) {
	// 		calc(node);
	// 	}
	// 	if (i == 3) {
	// 		break;
	// 	}
	// 	i++;
	// }





	fclose(fp);
	
	return 0;
}
