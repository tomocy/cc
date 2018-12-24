#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
    ND_NUM = 256,
    ND_IDENT,
};

typedef struct Node {
    int ty;
    struct Node* lhs;
    struct Node* rhs;
    int val;
    char name;
} Node;

enum {
    TK_NUM = 256,
    TK_IDENT,
    TK_EOF,
};

typedef struct {
    int ty;
    int val;
    char* input;
} Token;

extern Token tokens[100];

void tokenize(char* p);

void program();

extern Node* code[100];

void gen(Node* node);

void error(const char* format, ...);