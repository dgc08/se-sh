#include <string.h>

#include "se-target.h"
#include "builtin_funcs.h"

#define S_UTILS_IMPL
#include "s_utils.h"

typedef int (*Command_func)(char*);

typedef struct {
    const char *command;
    Command_func function;
} Command;

int se_exit_code = 0;

Command commands[] = {
    {"echo", sh_echo},
    {"exit", sh_exit},
    {"pass", sh_pass},
    {"sys", target_system},
    {"sesh", target_shell},
    {"stty", sh_stty},
    {"ord", sh_ord},
    {"clear", sh_clear},
    {"sleep", sh_sleep},
    {"cat", sh_cat},
    {"rm", sh_rm},
    {"ls", sh_ls},
    {"write", sh_write},
    {"exec", sh_exec},
    {NULLPTR, NULLPTR}
};

Command_func associate_builtin(char* command) {
    for (size_t i = 0; commands[i].command != NULLPTR; i++) {
        if (strcmp(command, commands[i].command) == 0) {
            return commands[i].function;
        }
    }
    return NULLPTR;
}

int prompt(char* inp) {
    Command_func func;
    char* command;
    
    if (inp == NULLPTR || inp[0] == '\0')
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
