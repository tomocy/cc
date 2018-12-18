#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "wrong number of parameters\n");
    }

    int arg = atoi(argv[1]);
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");
    printf("    mov rax, %d\n", arg);
    printf("    ret\n");
    return 0;
}