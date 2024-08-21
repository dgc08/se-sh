#include <string.h>

#include "se-target.h"
#include "utils.h"
#include "builtin_funcs.h"

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

  return NULL;
}

Command_func process_builtin(char** buf) {
  if (*buf[0] == '\0') {
    return sh_pass;
  }

  command = get_arg(buf);
  Command_func func = associate_builtin(command);

  return func;
}


int prompt(char* inp) {
    Command_func func = process_builtin(&inp);
    if (func) {
        se_exit_code = func(inp);
    }
    else {
        target_print(command);
        target_print(": command not found\n");
        se_exit_code = 1;
    }

    return se_exit_code;
}
