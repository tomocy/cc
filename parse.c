#include "cc.h"

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

        error("faild to tokenize: %s\n", p);
    }

    tokens[i].ty = TK_EOF;
    tokens[i].input = p;
}

Node* code[100];

int pos = 0;

Node* assign();
Node* expr();
Node* mul();
Node* term();
Node* new_node(int ty, Node* lhs, Node* rhs);
Node* new_node_num(int val);
Node* new_node_ident(char name);

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
        error("missing ;: %s\n", tokens[pos].input);
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
            error("missing ): %s", tokens[pos].input);
        }
        pos++;
        return node;
    }

    error("unexpected token: %s", tokens[pos].input);
    exit(1);
}

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