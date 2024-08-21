#define TARGET_LIBC

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "se-sh.h"
#include "se-target.h"

int ch;

void target_print(char* str) {
  printf(str);
}

void target_print_int(int num) {
  printf("%d", num);
}

void target_exit(int code) {
  exit(code);
}

int target_system(char* command) {
  int status = system(command);
  if (status == -1) {
        target_print("system call failed");
        return 1;
  }
  else {
        int exit_code = WEXITSTATUS(status);
        return exit_code;
  }
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


int main() {
  while (1) {
    printf("$ ");
    fflush(stdout);

    // Wait for user input
    char* inp = NULL;
    read_input (&inp);

    if (ch == EOF) {
      puts("");
      exit(0);
    }

    prompt(inp);
    free(inp);
  }
}
