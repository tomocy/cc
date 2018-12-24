#include <ctype.h>
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

Node* new_node_ident(char name) {
    Node* node = malloc(sizeof(Node));
    node->ty = ND_IDENT;
    node->name = name;
    return node;
}

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
            case '=':
            case ';':
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

        if ('a' <= *p && *p <= 'z') {
            tokens[i].ty = TK_IDENT;
            tokens[i].input = p;
            i++;
            p++;
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

Node* code[100];

int pos = 0;

Node* assign();
Node* expr();
Node* mul();
Node* term();

void program() {
    int i;
    for (i = 0; i < sizeof(code) / sizeof(Node*); i++) {
        if (tokens[pos].ty == TK_EOF) {
            break;
        }
        code[i] = assign();
    }

    code[i] = NULL;
}

Node* assign() {
    Node* lhs = expr();
    if (tokens[pos].ty == '=') {
        pos++;
        return new_node('=', lhs, assign());
    }
    if (tokens[pos].ty != ';') {
        fprintf(stderr, "missing ;: %s", tokens[pos].input);
        exit(1);
    }
    pos++;
    return lhs;
}

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
    if (tokens[pos].ty == ND_IDENT) {
        return new_node_ident(*(tokens[pos++].input));
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

void gen_lval(Node* node) {
    if (node->ty == ND_IDENT) {
        printf("    mov rax, rbp\n");
        printf("    sub rax, %d\n", ('z' - node->name + 1) * 8);
        printf("    push rax\n");
        return;
    }

    fprintf(stderr, "lvalue is not variable");
}

void gen(Node* node) {
    if (node->ty == ND_NUM) {
        printf("    push %d\n", node->val);
        return;
    }

    if (node->ty == ND_IDENT) {
        gen_lval(node);
        printf("    pop rax\n");
        printf("    mov rax, [rax]\n");
        printf("    push rax\n");
        return;
    }

    if (node->ty == '=') {
        gen_lval(node->lhs);
        gen(node->rhs);
        printf("    pop rdi\n");
        printf("    pop rax\n");
        printf("    mov [rax], rdi\n");
        printf("    push rdi\n");
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
    program();

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    printf("    push rbp\n");
    printf("    mov rbp, rsp\n");
    printf("    sub rsp, %d\n", 26 * 8);

    int i;
    for (i = 0; code[i]; i++) {
        gen(code[i]);
        printf("    pop rax\n");
    }

    printf("    mov rsp, rbp\n");
    printf("    pop rbp\n");
    printf("    ret\n");
    return 0;
}