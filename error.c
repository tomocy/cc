#include "cc.h"

void error(const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    fprintf(stderr, format, ap);
    va_end(ap);
    exit(1);
}