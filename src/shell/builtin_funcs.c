#include "builtin_funcs.h"
#include "se-target.h"
#include <string.h>

Terminal target_terminal;

int sh_exit(char* args) {
    int code = atoi(args); // I'll trust atoi is somewhere
    return 0-(code+1);
}

int sh_echo(char* args) {
    if (strcmp(args, "$?") == 0) {
        target_print_int(se_exit_code);
        target_newline();
        return 0;
    }
    target_print(args);
    target_newline();
    return 0;
}

int sh_stty(char* args) {
    if (strcmp(args, "-echo") == 0)
        target_terminal.echo = false;
    if (strcmp(args, "echo") == 0)
        target_terminal.echo = true;
    return 0;
}

int sh_ord(char* args) {
    for (; *args != '\0'; args++) {
        target_print_int(*args);
        target_print(" ");
    }
    target_newline();
    return 0;
}
