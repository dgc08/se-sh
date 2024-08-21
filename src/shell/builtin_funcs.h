#ifndef BUILTIN_FUNCS_H_
#define BUILTIN_FUNCS_H_

#include "se-target.h"

extern int se_exit_code;

int sh_exit(char* args);
int sh_echo(char* args);
int sh_stty(char* args);
static inline int sh_pass() {return 0;}

#endif // BUILTIN_FUNCS_H_
