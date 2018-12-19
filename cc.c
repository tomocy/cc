#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "wrong number of parameters\n");
    }

    char *arg = argv[1];
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");
    printf("    mov rax, %1d\n", strtol(arg, &arg, 10));

    while (*arg)
    {
        if (*arg == '+')
        {
            arg++;
            printf("    add rax, %1d\n", strtol(arg, &arg, 10));
            continue;
        }

        if (*arg == '-')
        {
            arg++;
            printf("    sub rax, %1d\n", strtol(arg, &arg, 10));
            continue;
        }

        fprintf(stderr, "unexpected operator '%c'\n", *arg);
        return 1;
    }

    printf("    ret\n");
    return 0;
}