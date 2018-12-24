#include "cc.h"

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