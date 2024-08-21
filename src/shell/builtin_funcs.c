#include "builtin_funcs.h"
#include "se-target.h"
#include <string.h>

int sh_exit(char* args) {
    int code = atoi(args); // I'll trust atoi is somewhere
    target_exit(code);
    return code;
}

int sh_echo(char* args) {
    if (strcmp(args, "$?") == 0) {
        target_print_int(se_exit_code);
        target_print("\n");
        return 0;
    }
    target_print(args);
    target_print("\n");
    return 0;
}
