#include <string.h>

#include "se-target.h"
#include "builtin_funcs.h"

#define S_UTILS_IMPL
#include "s_utils.h"

typedef int (*Command_func)(char*);

char* command;
int se_exit_code = 0;

Command_func associate_builtin(char* command) {
    if (strcmp(command, "echo") == 0) {
      return sh_echo;
    }
    else if (strcmp(command, "exit") == 0) {
      return sh_exit;
    }
    else if (strcmp(command, "pass") == 0) {
      return sh_pass;
    }
    else if (strcmp(command, "sys") == 0) {
      return target_system;
    }
    else if (strcmp(command, "sesh") == 0) {
      return target_shell;
    }
    else if (strcmp(command, "stty") == 0) {
      return sh_stty;
    }
    else if (strcmp(command, "ord") == 0) {
      return sh_ord;
    }
    else if (strcmp(command, "clear") == 0) {
      return sh_clear;
    }
    else if (strcmp(command, "sleep") == 0) {
      return sh_sleep;
    }
    else if (strcmp(command, "cat") == 0) {
      return sh_cat;
    }
    else if (strcmp(command, "rm") == 0) {
      return sh_rm;
    }
    else if (strcmp(command, "ls") == 0) {
      return sh_ls;
    }
    else if (strcmp(command, "write") == 0) {
      return sh_write;
    }
    else if (strcmp(command, "exec") == 0) {
      return sh_exec;
    }

    return NULL;
}

int prompt(char* inp) {
    Command_func func;
    
    if (inp == NULL || inp[0] == '\0')
      func = sh_pass;
    else {
      command = get_arg(&inp);
      func = associate_builtin(command);
    }

    if (func) {
      se_exit_code = func(inp);
    }
    else {
      se_exit_code = sh_exec(command);
    }

    if (se_exit_code < 0) {
      se_exit_code = (se_exit_code+1)*-1;
      return (se_exit_code+1)*-1;
    }

    return se_exit_code;
}
