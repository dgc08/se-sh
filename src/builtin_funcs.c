#include "builtin_funcs.h"

int sh_exit(char* args) {
    int code = atoi(args); // I'll trust atoi is somewhere
    target_exit(code);
    return code;
}

int sh_echo(char* args) {
    target_print(args);
    target_print("\n");
    return 0;
}
