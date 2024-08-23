#ifndef BUILTIN_FUNCS_H_
#define BUILTIN_FUNCS_H_

#include "se-target.h"

extern int se_exit_code;

int sh_exec(char* args);

int sh_exit(char* args);
int sh_echo(char* args);
int sh_stty(char* args);
int sh_ord(char* args);
int sh_sleep(char* args);
static inline int sh_clear() {
    target_print("\033[2J\033[H");
    return 0;
}

int sh_cat(char* args);
int sh_rm(char* args);
int sh_ls();
int sh_write(char* args);

static inline int sh_pass() {return 0;}

#endif // BUILTIN_FUNCS_H_
