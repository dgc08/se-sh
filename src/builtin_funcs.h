#ifndef BUILTIN_FUNCS_H_
#define BUILTIN_FUNCS_H_

#include <stdio.h>
#include <stdlib.h>

static inline int sh_exit(char* args) {
    exit(atoi(args));
}
static inline int sh_echo(char* args) {
    puts(args);
}

#endif // BUILTIN_FUNCS_H_
