#define TARGET_LIBC

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "se-sh.h"
#include "se-target.h"

#define MAX_HISTORY 100
#define BUFFER_SIZE 64

// Global variables
char* history[MAX_HISTORY];
int history_count = 0;
int current_history_index = -1;
struct termios orig_termios;

int ch;

extern Terminal target_terminal;

void target_newline(const char* str) {
    puts("");
}
void target_print(const char* str) {
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
};

// Function to reset terminal to original state
void reset_terminal_mode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}

// Function to set terminal to raw mode
void set_raw_mode() {
    struct termios raw;

    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(reset_terminal_mode);

    raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON | ISIG);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

// Function to handle arrow keys
int handle_arrow_keys() {
    char seq[3];
    if (read(STDIN_FILENO, &seq[0], 1) != 1) return 0;
    if (read(STDIN_FILENO, &seq[1], 1) != 1) return 0;
    if (seq[0] == '[') {
        switch (seq[1]) {
            case 'A': return 1; // Up arrow
            case 'B': return 2; // Down arrow
        }
    }
    return 0;
}

// Function to read user input
void read_input(char** input) {
    size_t buffer_size = BUFFER_SIZE;
    *input = (char *)malloc(buffer_size * sizeof(char));

    size_t length = 0;

    set_raw_mode();

    while (1) {
        char ch;
        if (read(STDIN_FILENO, &ch, 1) == -1) {
            break;
        }

        if (ch == '\n') {
            (*input)[length] = '\0';
            printf("\n");
            break;
        } else if (ch == 4) { // Handle EOF (Ctrl+D sends ASCII 4)
            puts("");
            target_exit(0);
            break;
        } else if (ch == 27) { // Escape sequence (potentially an arrow key)
            int arrow_key = handle_arrow_keys();
            if (arrow_key == 1 && history_count > 0) { // Up arrow
                if (current_history_index < history_count - 1) {
                    current_history_index++;
                }
                strcpy(*input, history[history_count - current_history_index - 1]);
                length = strlen(*input);
                printf("\r$ %s", *input);
                fflush(stdout);
            } else if (arrow_key == 2 && history_count > 0) { // Down arrow
                if (current_history_index > 0) {
                    current_history_index--;
                    strcpy(*input, history[history_count - current_history_index - 1]);
                } else {
                    current_history_index = -1;
                    length = 0;
                    (*input)[0] = '\0';
                }
                printf("\r$ %s", *input);
                fflush(stdout);
            }
        } else {
            if (length + 1 >= buffer_size) {
                buffer_size = (size_t)(buffer_size * 1.5);
                char *temp = realloc(*input, buffer_size * sizeof(char));
                if (!temp) {
                    fprintf(stderr, "Reallocation failed\n");
                    free(*input);
                    exit(1);
                }
                *input = temp;
            }
            (*input)[length++] = ch;
            (*input)[length] = '\0';
            printf("%c", ch);
            fflush(stdout);
        }
    }

    reset_terminal_mode();
}
// Function to add input to history
void add_to_history(char* input) {
    if (history_count < MAX_HISTORY) {
        history[history_count++] = strdup(input);
    } else {
        free(history[0]);
        for (int i = 1; i < MAX_HISTORY; i++) {
            history[i - 1] = history[i];
        }
        history[MAX_HISTORY - 1] = strdup(input);
    }
    current_history_index = -1;
}

int target_shell() {
    target_terminal.echo = true;
    while (1) {
        if (target_terminal.echo)
            printf("$ ");
        fflush(stdout);

        // Wait for user input
        char* inp = NULL;
        read_input(&inp);

        if (strlen(inp) > 0) {
            add_to_history(inp);
            // Handle input (this could be any function call)
            int code = prompt(inp);
            if (code < 0) {
                free(inp);
                return (code+1)*-1;
            }
        }

        free(inp);
    }

    return 0;
}

int main() {
    return target_shell();
}
