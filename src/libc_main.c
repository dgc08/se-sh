#include "sys/unistd.h"
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <stdint.h>
#include <sys/wait.h>
#include <string.h>
#include <termios.h>

#include "se-sh.h"
#include "se-target.h"

#define MAX_HISTORY 100
#define BUFFER_SIZE 16

// Global variables
char* history[MAX_HISTORY];
int history_count = 0;
int current_history_index = -1;
struct termios orig_termios;

char ch;

extern Terminal target_terminal;
void read_and_check_exit_condition();

void target_newline() {
    puts("");
}

void target_write_output(const char* bytes, size_t nbytes) {
    fwrite(bytes, 1, nbytes, stdout);
}

void target_print(const char* str) {
  printf("%s", str);
}

void target_print_int(int num) {
  printf("%d", num);
}

void target_sleep(uint32_t millis) {
    usleep(millis*1000);
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

void reset_terminal_mode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}

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

void clear_prompt(int len) {
    while (len > 0) {
        printf("\b\b");
        len--;
    }
}

// Function to read user input
void read_input(char** input) {
    size_t buffer_size = BUFFER_SIZE;
    *input = (char *)malloc(buffer_size * sizeof(char));

    size_t length = 0;

    set_raw_mode();
    while (1) {
        read_and_check_exit_condition();

        if (ch == '\n') {
            (*input)[length] = '\0';
            printf("\n");
            break;
        } if (ch == 27) { // Escape sequence (potentially an arrow key)
            int arrow_key = handle_arrow_keys();
            if (arrow_key == 1 && history_count > 0) { // Up arrow
                clear_prompt(length);
                if (current_history_index < history_count - 1) {
                    current_history_index++;
                }
                size_t new_length = strlen(history[history_count - current_history_index - 1]);

                // Check if buffer reallocation is needed
                if (new_length >= buffer_size) {
                    buffer_size = new_length + 1; // +1 for null terminator
                    *input = realloc(*input, buffer_size);
                    if (*input == NULL) {
                        perror("realloc");
                        exit(EXIT_FAILURE);
                    }
                }

                strcpy(*input, history[history_count - current_history_index - 1]);
                length = new_length;
                printf("%s", *input);
                fflush(stdout);
            } else if (arrow_key == 2 && history_count > 0) { // Down arrow
                clear_prompt(length);
                if (current_history_index > 0) {
                    current_history_index--;
                    size_t new_length = strlen(history[history_count - current_history_index - 1]);

                    // Check if buffer reallocation is needed
                    if (new_length >= buffer_size) {
                        buffer_size = new_length + 1; // +1 for null terminator
                        *input = realloc(*input, buffer_size);
                        if (*input == NULL) {
                            perror("realloc");
                            exit(EXIT_FAILURE);
                        }
                    }

                    strcpy(*input, history[history_count - current_history_index - 1]);
                    length = new_length;
                } else {
                    current_history_index = -1;
                    length = 0;
                    (*input)[0] = '\0';
                }
                printf("%s", *input);
                fflush(stdout);
            }
        } else if (ch == 8 || ch == 127) { // Backspace
            if (length > 0) {
                (*input)[--length] = '\0';
                printf("\b \b");
                fflush(stdout);
            }
        } else {
            if (length + 1 >= buffer_size) {
                buffer_size = (size_t)(buffer_size * 2);
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

void read_and_check_exit_condition() {
    if (read(STDIN_FILENO, &ch, 1) == -1 || ch == 4 || ch == EOF) {
        puts("");
        target_exit(130);
    }
}

// TODO: This doesn't work
void target_check_exit_condition() {
    char ch;

    // Set up the pollfd structure
    struct pollfd fds;
    fds.fd = STDIN_FILENO;
    fds.events = POLLIN; // Check for readable events

    // Poll with a timeout of 0 milliseconds to check for input availability
    int ret = poll(&fds, 1, 0);

    // Check if poll indicates data is available to read
    if (ret > 0 && (fds.revents & POLLIN)) {
        ssize_t n = read(STDIN_FILENO, &ch, 1);
        if (n == 1) {
            // Check the exit conditions
            if (ch == 4 || ch == EOF) {
                puts("");
                target_exit(132);
            }
            else {
                printf("REad shit: %c", ch);
            }
        }
    } else if (ret < 0) {
        // Handle poll error
        perror("poll");
    }
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

size_t target_write_file(const char* filename, Container contents) {
    FILE* file = fopen(filename, "wb");
    if (!file)
        return 0;

    size_t w = fwrite(contents.content, 1, contents.size, file);
    fclose(file);
    return w;
}

Container target_read_file(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file)
        return (Container){0, 0};

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);

    void* ret = malloc(size);
    if (!ret) {
        fclose(file);
        return (Container){0, 0};
    }

    size_t read = fread(ret, 1, size, file);
    fclose(file);

    if (read != size) {
        free(ret);
        return (Container){0, 0};
    }

    return (Container){ret, size};
}

Container target_list_files() {
    DIR* dir = opendir(".");
    if (!dir)
        return (Container){0, 0};

    size_t size = 1;
    struct dirent* entry;
    while ((entry = readdir(dir)))
        size += strlen(entry->d_name) + 1;

    rewinddir(dir);

    char* ret = (char*)malloc(size);
    if (!ret) {
        closedir(dir);
        return (Container){0, 0};
    }

    ret[0] = '\0';
    while ((entry = readdir(dir))) {
        strcat(ret, entry->d_name);
        strcat(ret, "\n");
    }

    closedir(dir);
    return (Container){ret, size};
}

bool target_remove_file(const char* filename) {
    return unlink(filename) == 0;
}

int main() {
    puts("Welcome to se-sh!");
    return target_shell();
}
