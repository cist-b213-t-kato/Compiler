#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define LINE_MAX 256

enum Type {
	NUM,
	STRING,
	ADD_OPE,
	SUB_OPE,
	MUL_OPE,
	DIV_OPE,
	OPE_EQUAL,
	VAR_TYPE,
	IF,
	ELSE,
	IDENT,
	BRACKET_START,
	BRACKET_END,
	BLOCK_START,
	BLOCK_END,
	PRINT_INT,
	PRINT_STRING,
	END
};

struct Token {
	enum Type type;
	void* data;
};

struct Node {
	void *data;
	struct Node *left;
	struct Node *right;
};

struct Value {
	enum Type type;
	void *data;
};

struct Map {
	char *key[100];
	void *value[100];
	int size;
};

struct Token *tokens[100];
int tokensLength;
int pos;

struct Map *map;

struct Node *ParseExpression();

void putMap(char *key, void *value) {
	int i;
	for (i = 0; i < map->size; i++) {
		if (strcmp(map->key[i], key) == 0) {
			map->value[i] = value;
			return;
		}
	}
	// map->key[map->size] = value;
	map->key[map->size] = key;
	map->value[map->size] = value;
	map->size++;
}

void *getMap(char *key) {
	int i;
	for (i = 0; i < map->size; i++) {
		if (strcmp(map->key[i], key) == 0) {
			return map->value[i];
		}
	}
	return NULL;
}

struct Token *NewToken(enum Type type, void* data) {
	struct Token *tokenTmp;
	tokenTmp = (struct Token *)malloc(sizeof(struct Token));
	tokenTmp->type = type;
	tokenTmp->data = data;
	return tokenTmp;
}

void SkipComment(char **ps) {
	if (strncmp(*ps, "//", 2) == 0) {
		while (**ps != '\0' && **ps != '\n') {
			*ps += 1;
		}
	} else if (strncmp(*ps, "/*", 2) == 0) {
		while (strncmp(*ps, "*/", 2) == 0) {
			if (**ps != '\0') {
				printf("ERROR\n");
				exit(-1);
			}
			*ps += 1;
		}
	}
}

struct Token *MakeToken(char **ps) {
	int i;
	char *sData;
	char sBuf[256];

	// printf("%c", **ps);

	if (isspace(**ps)) {
		*ps += 1;
		return NULL;
	}

	if (isdigit(**ps)) {
		// tokens[tokensLength] = NewToken(NUM, (void *)malloc(sizeof(int)));
		// *(int *)(tokens[tokensLength]->data) = strtol(s, &s, 10);
		return NewToken(NUM, (void *)strtol(*ps, ps, 10));
	}

	if (strncmp(*ps, "if", 2) == 0) {
		if (!isalnum(*( *ps + 2 ))) {
			*ps += 2;
			return NewToken(IF, NULL);
		}
	}

	if (strncmp(*ps, "else", 4) == 0) {
		if (!isalnum(*( *ps + 4 ))) {
			*ps += 4;
			return NewToken(ELSE, NULL);
		}
	}

	if (strncmp(*ps, "printInt", 8) == 0) {
		if (!isalnum(*( *ps + 8 ))) {
			*ps += 8;
			return NewToken(PRINT_INT, NULL);
		}
	}

	if (strncmp(*ps, "printString", 11) == 0) {
		if (!isalnum(*( *ps + 11 ))) {
			*ps += 11;
			return NewToken(PRINT_STRING, NULL);
		}
	}

	if (strncmp(*ps, "int", 3) == 0) {
		if (!isalnum(*( *ps + 3 ))) {
			*ps += 3;
			return NewToken(VAR_TYPE, "int");
		}
	}

	if (strncmp(*ps, "char*", 5) == 0) {
		if (!isalnum(*( *ps + 5 ))) {
			*ps += 5;
			return NewToken(VAR_TYPE, "char*");
		}
	}

	// 識別子
	if (isalpha(**ps)) {
		i = 1;
		while (isalnum(*(*ps + i))) {
			i++;
		}
		sData = (char *)malloc(sizeof(char) * i);
		strncpy(sData, *ps, i);
		*ps += i;
		return NewToken(IDENT, (void *)sData);
	}

	if (**ps == '"') {
		*ps += 1;
		i = 0;
		while (**ps != '"') {
			if (strncmp(*ps, "\\n", 2) == 0) {
				sBuf[i] = '\n';
				*ps += 2;
			} else {
				sBuf[i] = **ps;
				*ps += 1;
			}
			i++;
		}
		*ps += 1;
		sBuf[i] = '\0';
		sData = (char *)malloc(sizeof(char) * (i + 1));
		strncpy(sData, sBuf, i);
		return NewToken(STRING, (void *)sData);
	}

	if (**ps == '{') {
		*ps += 1;
		return NewToken(BLOCK_START, NULL);
	}

	if (**ps == '}') {
		*ps += 1;
		return NewToken(BLOCK_END, NULL);
	}

	if (**ps == '(') {
		*ps += 1;
		return NewToken(BRACKET_START, NULL);
	}

	if (**ps == ')') {
		*ps += 1;
		return NewToken(BRACKET_END, NULL);
	}

	if (**ps == '=') {
		*ps += 1;
		return NewToken(OPE_EQUAL, NULL);
	}

	if (**ps == '+') {
		*ps += 1;
		return NewToken(ADD_OPE, NULL);
	}

	if (**ps == '-') {
		*ps += 1;
		return NewToken(SUB_OPE, NULL);
	}

	if (**ps == '*') {
		*ps += 1;
		return NewToken(MUL_OPE, NULL);
	}

	if (**ps == '/') {
		*ps += 1;
		return NewToken(DIV_OPE, NULL);
	}

	if (**ps == ';') {
		*ps += 1;
		return NewToken(END, NULL);
	}

	printf("不正な文字列です。");
	exit(-1);
}

void tokenize(char *s) {
	tokensLength = 0;
	struct Token *pkToken;

	// printf("%s\n", s);
	while (*s != '\0') {
		SkipComment(&s);

		pkToken = MakeToken(&s);
		if (pkToken != NULL) {
			tokens[tokensLength++] = pkToken;
		}
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
	if (token->type == NUM) printf("NUM : %ld", (long)token->data);
	if (token->type == STRING) printf("STRING : %s", token->data);
	if (token->type == ADD_OPE) printf("ADD_OPE");
	if (token->type == SUB_OPE) printf("SUB_OPE");
	if (token->type == MUL_OPE) printf("MUL_OPE");
	if (token->type == DIV_OPE) printf("DIV_OPE");
	if (token->type == OPE_EQUAL) printf("OPE_EQUAL");
	if (token->type == VAR_TYPE) printf("VAR_TYPE : %s", token->data);
	if (token->type == IDENT) printf("IDENT : %s", (char *)token->data);
	if (token->type == IF) printf("IF");
	if (token->type == ELSE) printf("ELSE");
	if (token->type == BRACKET_START) printf("(");
	if (token->type == BRACKET_END) printf(")");
	if (token->type == BLOCK_START) printf("{");
	if (token->type == BLOCK_END) printf("}");
	if (token->type == PRINT_INT) printf("PRINT_INT");
	if (token->type == PRINT_STRING) printf("PRINT_STRING");
	if (token->type == END) printf(";");
	printf("\n");
}

void printTokens() {
	int i;
	printf("[print tokens]\n");
	for (i = 0; i < tokensLength; i++) {
		printf("%2d: ", i);
		printToken(tokens[i]);
	}
}

struct Token *Consume(enum Type type) {
	// pos < tokensLength なくてもイケる？
	if (pos < tokensLength && tokens[pos]->type == type) {
		return tokens[pos++];
	}
	return NULL;
}

struct Node *ParseFactor() {
	struct Token *token;
	struct Node *node;

	if ((token = Consume(PRINT_INT)) != NULL) {
		return NewNode((void *)token, NULL, ParseExpression());
	} else if ((token = Consume(PRINT_STRING)) != NULL) {
		return NewNode((void *)token, NULL, ParseExpression());
	} else if ((token = Consume(NUM)) != NULL) {
		return NewNode((void *)token, NULL, NULL);
	} else if ((token = Consume(STRING)) != NULL) {
		return NewNode((void *)token, NULL, NULL);
	} else if ((token = Consume(IDENT)) != NULL) {
		// printf("%s\n", "ParseFactor_IDENT");
		node = NewNode((void *)token, NULL, NULL);
		if ((token = Consume(OPE_EQUAL)) != NULL) {
			// printf("%s\n", "ParseFactor_OPE_EQUAL");
			node = NewNode((void *)token, node, ParseExpression());
		}
		return node;
	} else {
		// printf("hoge\n");
		// printf("%d\n", token == NULL);
		// printToken(token);
		// printf("syntax error : not ParseFactor.\n");
		return NULL;
	}
}

struct Node *ParseTerm() {
	struct Node *node;
	struct Token *token;

	node = ParseFactor();

	// printToken(getToken(node));

	while (1) {
		if ((token = Consume(MUL_OPE)) != NULL || (token = Consume(DIV_OPE)) != NULL) {
			node = NewNode((void *)token, node, ParseFactor());
		} else {
			return node;
		}
	}
}

struct Node *ParseExpression() {
	struct Node *node;
	struct Token *token;

	node = ParseTerm();

	while (1) {
		if ((token = Consume(ADD_OPE)) != NULL || (token = Consume(SUB_OPE)) != NULL) {
			node = NewNode((void *)token, node, ParseTerm());
		} else {
			return node;
		}
	}
}

struct Value calc(struct Node *node) {
	enum Type type = ((struct Token *)node->data)->type;
	struct Value ret;
	struct Value value;
	struct Value *pRet;
	struct Value lResult;
	struct Value rResult;

	if (getToken(node)->type == END) {
		if (node->left != NULL) {
			// execute(node->left);
			calc(node->left);
		}

		if (node->right != NULL) {
			// execute(node->right);
			calc(node->right);
		}
		return ret;
	} else if (getToken(node)->type == VAR_TYPE) {
		// printf("%s\n", getToken(node)->data);
		if (strcmp(getToken(node)->data, "int") == 0) {
			// printf("%s\n", getToken(node->right)->data);
			putMap(getToken(node)->data, (void *)0);
		}
		return ret;
	} else if (getToken(node)->type == ELSE) {
		// ELSE_IF_THEN
		if (getToken(node->left)->type == IF) {
			ret = calc(node->left);
		}
		if (!ret.data && (node->right != NULL)) {
			calc(node->right);
		}
		return ret;
	} else if (getToken(node)->type == IF) {
		// IF_THEN
		// printf("IF\n");
		// printToken(getToken(node->left));
		// if (isExpr(node->left)) {
		ret = calc(node->left);
		// printf("%d\n", value.data);
		// }
		if (ret.data && (node->right != NULL)) {
			calc(node->right);
		}
		return ret;
	} else if (type == NUM) {
		// return *(int *)((struct Token *)node->data)->data;
		ret.type = NUM;
		ret.data = getToken(node)->data;
		return ret;
	} else if (type == STRING) {
		// return *(int *)((struct Token *)node->data)->data;
		ret.type = STRING;
		// printf("piyo");
		// printf("%s\n", getToken(node)->data);
		ret.data = getToken(node)->data;
		return ret;
	} else if (type == OPE_EQUAL) {
		pRet = (struct Value *)malloc(sizeof(struct Value));
		pRet->type = calc(node->right).type;
		pRet->data = calc(node->right).data;
		// printf("calc_OPE_EQUAL %s %d\n",
		// 	getToken(node->left)->data, *(int *)getToken(node->right)->data);
		putMap(getToken(node->left)->data, pRet);
		return *pRet;
	} else if (type == IDENT) {
		// printf("calc_IDENT ");
		// printf("%s ", (char *)getToken(node)->data);
		// printf("%d\n", *(int *)getMap((char *)getToken(node)->data));
		// printf("%d\n", getMap("abc"));
		// return 1000;
		ret.type = ((struct Value *)getMap((char *)getToken(node)->data))->type;
		ret.data = ((struct Value *)getMap((char *)getToken(node)->data))->data;
		return ret;
	} else if (type == PRINT_INT) {
		// printf("print\n");
		// printToken(getToken(node->right));
			// printf("calc_PRINT %d\n", *(int *)getMap("abc"));
		value = calc(node->right);
		printf("%ld", (long)value.data);
		return ret;
	} else if (type == PRINT_STRING) {
		// printf("print\n");
		// printToken(getToken(node->right));
			// printf("calc_PRINT %d\n", *(int *)getMap("abc"));
		value = calc(node->right);
		printf("%s", (char *)value.data);
		return ret;
	} else {
		lResult = calc(node->left);
		rResult = calc(node->right);
		ret.type = NUM;
		if (type == ADD_OPE) {
			ret.data = (void *)((long)lResult.data + (long)rResult.data);
		} else if (type == SUB_OPE) {
			ret.data = (void *)((long)lResult.data - (long)rResult.data);
		} else if (type == MUL_OPE) {
			ret.data = (void *)((long)lResult.data * (long)rResult.data);
		} else if (type == DIV_OPE) {
			ret.data = (void *)((long)lResult.data / (long)rResult.data);
		}
		return ret;
	}

	printf("不正なノード\n");
	// printToken(getToken(node));
	return ret;
}

// void skipBlock() {
// 	if (!consume(BLOCK_START)) {
// 		return;
// 	}

// 	while (!consume(BLOCK_END)) {
// 		if (consume(BLOCK_START)) {
// 			skipBlock();
// 		}
// 		pos++;
// 	}
// 	pos++;
// }

struct Node *stmt() {
	struct Node *node;
	struct Token *token;
	// struct Node *ret;

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
	// 	node = ParseExpression();

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

	// IF
	if ((token = Consume(IF)) != NULL) {
		// printf("IF\n");
		if (Consume(BRACKET_START) == NULL) {
			printf("syntax error : Write '(' after if.\n");
			exit(-1);
		}

		node = ParseExpression();

		if (Consume(BRACKET_END) == NULL) {
			printf("syntax error : Write ')' after ParseExpression.\n");
			exit(-1);
		}

		node = NewNode((void *)token, node, stmt());

		if ((token = Consume(ELSE)) != NULL) {
			node = NewNode((void *)token, node, stmt());
		}

		// TODO 違うだろお？
		token = NewToken(END, NULL);

		node = NewNode((void *)token, NULL, node);

		return node;
	}

	// 型宣言
	if ((token = Consume(VAR_TYPE)) != NULL) {
		node = NewNode((void *)token, NULL, NULL);
		if((token = Consume(IDENT)) != NULL) {
			node->right = NewNode((void *)token, NULL, NULL);
		}

		if ((token = Consume(END)) != NULL) {
			// printToken(token);
			return NewNode((void *)token, NULL, node);
		} else {
			printf("セミコロンが抜けています\n");
			exit(-1);
		}
	}

	node = ParseExpression();

	if ((token = Consume(END)) != NULL) {
		// printToken(token);
		// return NewNode((void *)token, stmt(), node);
		return NewNode((void *)token, NULL, node);
	} else {
		printf("セミコロンが抜けています\n");
		exit(-1);
	}

	// printf("syntax error : Unknown syntax.\n");

	return NULL;
}

// struct Node *program() {
// 	struct Node *child;
// 	struct Node *parent;

// 	child = stmt(); // A
// 	while (pos < tokensLength) {
// 		parent = stmt(); // B
// 		parent->left = child; // B->left = A
// 		child = parent; // A = B
// 	}
// 	return child;
// }

// int isExpr(struct Node *node) {
// 	enum Type type = getToken(node)->type;
// 	if (type == ADD_OPE || type == SUB_OPE
// 			|| type == MUL_OPE || type == DIV_OPE
// 			|| type == NUM || type == OPE_EQUAL
// 			|| type == IDENT) {
// 		return 1;
// 	}
// 	return 0;
// }

int ReadSource(FILE *fp, char *s) {
	int iSize;
	char sLine[256];

	iSize = 0;
	// while (fscanf(fp, "%s\n", sLine[iSize]) != EOF) {
	// while (fscanf(fp, "%s", sLine[iSize]) != EOF) {
	// TODO これはひどい
	while (fgets(sLine, 256, fp) != NULL) {
		strcat(s, sLine);
		iSize++;
	}

	fclose(fp);

	return 0;
}

int main(int argv, char *args[]) {
	char *s;
	struct Node *node;
	FILE *fp;

	if (argv != 2) {
		printf("Illegal argument error.\n");
		exit(1);
	}

	s = (char *)malloc(sizeof(char) * 4096);

	fp = fopen(args[1], "r");
	if (fp == NULL) {
		return -1;
	}
	
	if (ReadSource(fp, s) == -1) {
		exit(-1);
	}

	// printf("%s\n", s);

	tokenize(s);

	printTokens();
	printf("----------\n");

	pos = 0;

	map = (struct Map *)malloc(sizeof(struct Map));
	map->size = 0;

	// ParseExpression();

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

	while (pos < tokensLength) {
		node = stmt();
		// node = program();
		// printf("%d\n", calc(node->right->left));
		// printf("%d\n", calc(node->left->right->left));
		// printf("execute_node\n");
		// printToken(getToken(node));
		calc(node);
	}

	// putMap("x", (void *)1);

	// if (getMap("abc") != NULL) {
	// 	printf("main_getMap = %d\n", *(int *)getMap("abc"));
	// }

	// printf("hogehoge\n");
	
	return 0;
}
