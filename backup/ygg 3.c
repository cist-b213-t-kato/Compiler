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
			continue;
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
	if (token->type == SUB_OPE) printf("SUB_OPE");
	if (token->type == MUL_OPE) printf("MUL_OPE");
	if (token->type == DIV_OPE) printf("DIV_OPE");
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
		// printf("hoge\n");
		// printf("%d\n", token == NULL);
		// printToken(token);
		// printf("syntax error : not term.\n");
		return NULL;
	}
}

struct Node *mul() {
	struct Node *node = term();
	struct Token *token;

	// printToken(getToken(node));

	while (1) {
		if (consume(MUL_OPE) || consume(DIV_OPE)) {
			token = tokens[pos];
			// printToken(token);
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
		if (consume(ADD_OPE) || consume(SUB_OPE)) {
			token = tokens[pos];
			pos++;
			node = NewNode((void *)token, node, mul());
		} else {
			return node;
		}
	}
}

int calc(struct Node *node) {
	enum TokenType type = ((struct Token *)node->data)->type;
	int lResult;
	int rResult;
	if (type == NUM) {
		// return *(int *)((struct Token *)node->data)->data;
		return *(int *)getToken(node)->data;
	} else {
		// if (type == PRINT) {
		// 	ret = calc(node->right);
		// 	if (getToken(ret)->type == NUM) {
		// 		printf("%d", *(int *)getToken(ret)->data);
		// 	}
		// 	// printf("hogehogehoge\n");
		// 	return NULL;
		// }
		lResult = calc(node->left);
		rResult = calc(node->right);
		if (type == ADD_OPE) {
			return lResult + rResult;
		} else if (type == SUB_OPE) {
			return lResult - rResult;
		} else if (type == MUL_OPE) {
			return lResult * rResult;
		} else if (type == DIV_OPE) {
			return lResult / rResult;
		} else {
			return 0;
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
	// struct Node *ret;

	if (consume(PRINT)) {
		token = tokens[pos];
		pos++;
		// ret = calc(expr());
		// if (((struct Token *)ret->data)->type == NUM) {
		// 	printf("NUM : %d\n", *(int *)((struct Token *)ret->data)->data);
		// }
		node = NewNode((void *)token, expr(), NULL);
		// return NULL;
		if (consume(END)) {
			token = tokens[pos];
			pos++;
			// printToken(token);
			return NewNode((void *)token, NULL, node);
		}
	}

	// if (consume(BLOCK_START)) {
	// 	pos++;
	// 	while (!consume(BLOCK_END)) {
	// 		node = stmt();
	// 		if (node != NULL) {
	// 			calc(node);
	// 		}
	// 	}
	// 	pos++;
	// 	return NULL;
	// }

	// if (consume(IF)) {
	// 	token = tokens[pos];
	// 	pos++;

	// 	if (!consume(BRACKET_START)) {
	// 		printf("syntax error : Write a bracket after if.\n");
	// 		exit(-1);
	// 	}

	// 	pos++;
	// 	node = expr();

	// 	if (consume(BRACKET_END)) {
	// 		pos++;
	// 	}

	// 	ret = calc(node);

	// 	node = stmt();

	// 	if (*(int *)getToken(ret)->data != 0) {
	// 		// else節を読み飛ばす
	// 		if (consume(ELSE)) {
	// 			pos++;
	// 			if (consume(BLOCK_START)) {
	// 				skipBlock();
	// 				printToken(tokens[pos]);
	// 			} else {
	// 				// printToken(tokens[pos]);
	// 				stmt();
	// 			}
	// 		}
	// 		return node;
	// 	} else {
	// 		if (consume(ELSE)) {
	// 			pos++;
	// 			return stmt();
	// 		}
	// 	}
	// 	return NULL;
	// }

	if (consume(IF)) {
		token = tokens[pos];
		pos++;

		if (!consume(BRACKET_START)) {
			printf("syntax error : Write '(' after if.\n");
			exit(-1);
		}
		pos++;
		node = expr();

		if (!consume(BRACKET_END)) {
			printf("syntax error : Write ')' after expr.\n");
			exit(-1);
		}
		pos++;

		node = NewNode((void *)token, node, stmt());

		if (consume(ELSE)) {
			token = tokens[pos];
			pos++;
			node = NewNode((void *)token, node, stmt());
		}

		// TODO 違うだろお？
		token = NewToken(END, NULL);

		node = NewNode((void *)token, NULL, node);

		return node;
	}

	node = expr();

	if (consume(END)) {
		token = tokens[pos];
		pos++;
		// printToken(token);
		return NewNode((void *)token, stmt(), node);
	}

	// printf("syntax error : Unknown syntax.\n");

	return NULL;
}

struct Node *program() {
	struct Node *child;
	struct Node *parent;

	child = stmt(); // A
	while (pos < tokensLength) {
		parent = stmt(); // B
		parent->left = child; // B->left = A
		child = parent; // A = B
	}
	return child;
}

int isExpr(struct Node *node) {
	enum TokenType type = getToken(node)->type;
	if (type == ADD_OPE || type == SUB_OPE
			|| type == MUL_OPE || type == DIV_OPE
			|| type == NUM) {
		return 1;
	}
	return 0;
}

int execute(struct Node *node) {
	int ret = 1;

	if (getToken(node)->type == END) {
		if (node->left != NULL) {
			execute(node->left);
		}

		if (node->right != NULL) {
			execute(node->right);
		}
	}

	// ELSE_IF_THEN
	if (getToken(node)->type == ELSE) {
		if (getToken(node->left)->type == IF) {
			ret = execute(node->left);
		}
		if (!ret && (node->right != NULL)) {
			execute(node->right);
		}
		return !ret;
	}

	// IF_THEN
	if (getToken(node)->type == IF) {
		// printf("IF\n");
		// printToken(getToken(node->left));
		if (isExpr(node->left)) {
			// printf("isExpr\n");
			ret = calc(node->left);
		}
		if (ret && (node->right != NULL)) {
			execute(node->right);
		}	
		return ret;
	}

	if (getToken(node)->type == PRINT) {
		// printf("print\n");
		// printToken(getToken(node->right->left));
		if (isExpr(node->left)) {
			ret = calc(node->left);
			printf("%d\n", ret);
		}
	}

	return 1;
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
	// while (fscanf(fp, "%s\n", psBuf[iSize]) != EOF) {
	// while (fscanf(fp, "%s", psBuf[iSize]) != EOF) {
	// TODO これはひどい
	while (fgets(psBuf[iSize], 256, fp) != NULL) {
		for (i = 0; i < 256 - 2; i++) { 
			if (strncmp(psBuf[iSize] + i, "//", 2) == 0) {
				strcpy(psBuf[iSize] + i, "");
				break;
			}
		}
		iSize++;
	}

	for (i = 0; i < iSize; i++) {
		strcat(s, psBuf[i]);
		strcat(s, " ");
	}

	// printf("%s\n", s);

	tokenize(s);

	// printTokens();
	// printf("----------\n");

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

	node = program();
	// printf("%d\n", calc(node->right->left));
	// printf("%d\n", calc(node->left->right->left));
	execute(node);
	// printToken(getToken(node));

	fclose(fp);
	
	return 0;
}
