#ifndef BUILTIN_FUNCS_H_
#define BUILTIN_FUNCS_H_

#include "se-target.h"

int sh_exit(char* args);
int sh_echo(char* args);
static inline int sh_pass(char* args) {return 0;}

#endif // BUILTIN_FUNCS_H_
