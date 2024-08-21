#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "utils.h"
#include "builtin_funcs.h"

int ch;

typedef int (*Command_func)(char*);

Command_func associate_builtin(char* command) {
  if (strcmp(command, "echo") == 0) {
    return sh_echo;
  }
  else if (strcmp(command, "exit") == 0) {
    return sh_exit;
  }

  return NULL;
}

void read_input(char** input) {
    size_t buffer_size = 64;
    *input = (char *)malloc(buffer_size * sizeof(char));
    if (!*input) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    size_t length = 0;

    while ((ch = fgetc(stdin)) != '\n' && ch != EOF) {
        if (length + 1 >= buffer_size) {  // +1 for the null terminator
            buffer_size = (size_t)(buffer_size * 1.5);
            char *temp = realloc(*input, buffer_size * sizeof(char));
            if (!temp) {
                fprintf(stderr, "Reallocation failed\n");
                free(*input);
                exit(1);
            }
            *input = temp;
        }
        (*input)[length++] = (char)ch;
    }

    (*input)[length] = '\0';  // Null-terminate the string
}

bool process_builtin(char* buf) {
  if (buf[0] == '\0') {
    if (ch == EOF) {
      puts("");
      exit(0);
    }
    return true;
  }

  char* command = get_arg(&buf);
  Command_func func = associate_builtin(command);

  if (func != NULL) {
    func(buf);
  }
  else {
    printf("%s: command not found\n", command);
  }

  return true;
}


void prompt() {
  printf("$ ");
  fflush(stdout);

  // Wait for user input
  char* inp = NULL;
  read_input (&inp);

  if (process_builtin(inp))

  free (inp);
}

int main() {
  while (1) {
    prompt();
  }
}
