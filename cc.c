#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
    ND_NUM = 256,
};

typedef struct Node {
    int ty;
    struct Node* lhs;
    struct Node* rhs;
    int val;
} Node;

Node* new_node(int ty, Node* lhs, Node* rhs) {
    Node* node = malloc(sizeof(Node));
    node->ty = ty;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node* new_node_num(int val) {
    Node* node = malloc(sizeof(Node));
    node->ty = ND_NUM;
    node->val = val;
    return node;
}

enum {
    TK_NUM = 256,
    TK_EOF,
};

typedef struct {
    int ty;
    int val;
    char* input;
} Token;

Token tokens[100];

void tokenize(char* p) {
    int i = 0;
    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        switch (*p) {
            case '+':
            case '-':
            case '*':
            case '/':
            case '(':
            case ')':
                tokens[i].ty = *p;
                tokens[i].input = p;
                i++;
                p++;
                continue;
        }

        if (isdigit(*p)) {
            tokens[i].ty = TK_NUM;
            tokens[i].val = strtol(p, &p, 10);
            tokens[i].input = p;
            i++;
            continue;
        }

        fprintf(stderr, "faild to tokenize: %s\n", p);
        exit(1);
    }

    tokens[i].ty = TK_EOF;
    tokens[i].input = p;
}

void error(int i) {
    fprintf(stderr, "unexpected token: %s\n", tokens[i].input);
    exit(1);
}

int pos = 0;

Node* expr();
Node* mul();
Node* term();

Node* expr() {
    Node* lhs = mul();
    if (tokens[pos].ty == '+') {
        pos++;
        return new_node('+', lhs, expr());
    }
    if (tokens[pos].ty == '-') {
        pos++;
        return new_node('-', lhs, expr());
    }

    return lhs;
}

Node* mul() {
    Node* lhs = term();
    if (tokens[pos].ty == '*') {
        pos++;
        return new_node('*', lhs, mul());
    }
    if (tokens[pos].ty == '/') {
        pos++;
        return new_node('/', lhs, mul());
    }

    return lhs;
}

Node* term() {
    if (tokens[pos].ty == ND_NUM) {
        return new_node_num(tokens[pos++].val);
    }
    if (tokens[pos].ty == '(') {
        pos++;
        Node* node = expr();
        if (tokens[pos].ty != ')') {
            fprintf(stderr, "missing ): %s", tokens[pos].input);
            exit(1);
        }
        pos++;
        return node;
    }

    fprintf(stderr, "unexpected token: %s", tokens[pos].input);
}

void gen(Node* node) {
    if (node->ty == ND_NUM) {
        printf("    push %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch (node->ty) {
        case '+':
            printf("    add rax, rdi\n");
            break;
        case '-':
            printf("    sub rax, rdi\n");
            break;
        case '*':
            printf("    mul rdi\n");
            break;
        case '/':
            printf("    mov rdx, 0\n");
            printf("    div rdi\n");
            break;
    }

    printf("    push rax\n");
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "wrong number of parameters\n");
    }

    tokenize(argv[1]);
    Node* node = expr();

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    gen(node);

    printf("    pop rax\n");
    printf("    ret\n");
    return 0;
}