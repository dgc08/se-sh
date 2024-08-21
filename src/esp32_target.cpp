#include "Arduino.h"

#include "se-target.h"
#include "se-sh.h"

#define MAX_HISTORY 16

String history[MAX_HISTORY];
int history_cursor = 0;

int exit_code;
bool running;
Terminal target_terminal;

String input_buffer;

void target_print(const char* str) {
    Serial.print(str);
}

void target_print_int(int num) {
    Serial.print(num);
}

void target_exit(int code) {
    exit_code = code;
    running = false;
}

int target_system(char* command) {
    return 0;
}

void target_newline() {
    Serial.println();
}

void clear_prompt() {
    while (input_buffer.length()) {
        target_print("\b \b");
        input_buffer.remove(input_buffer.length() - 1);
    }
}

int target_shell() {
    target_terminal.echo = true;

    static bool new_iter;
    static char old_char = 0;
    static int scroll_cursor = 0;
    static bool scrolling = false;
    running = true;

    Serial.println("Welcome to se-sh!");
    new_iter = true;

    while (running) {
        if (new_iter) {
            new_iter = false;
            scrolling = false;

            input_buffer = "";
            if (target_terminal.echo)
                Serial.print("$ ");
        }
        if (!scrolling)
            scroll_cursor = history_cursor;
        if (Serial.available() > 0) {
            char rchar = Serial.read();

            if (rchar == '\r' || (rchar == '\n' && old_char != '\r')) {
                if (target_terminal.echo)
                    target_newline(); // flush

                history[history_cursor] = input_buffer;
                history_cursor++;
                history_cursor %= MAX_HISTORY;
                
                char mutableInput[input_buffer.length() + 1];
                input_buffer.toCharArray(mutableInput, input_buffer.length() + 1);

                int code = prompt(mutableInput);
                if (code < 0) {
                    return (code+1)*-1;
                }
                new_iter = true;
            }
            else if (rchar == '\n') {} // ignore
            else if (rchar == 0x8 || rchar == 0x7f) {
                if (input_buffer.length())
                    target_print("\b \b");
                input_buffer.remove(input_buffer.length() - 1);
            }
            else if (rchar == 0x4) {
                target_newline();
                target_print("exit");
                if (target_terminal.echo)
                    Serial.print(rchar);
                return 0;
            }
            else if (rchar == 16) {
                scroll_cursor--;
                scroll_cursor %= MAX_HISTORY;
                clear_prompt();

                if (history[scroll_cursor] == nullptr)
                    input_buffer = "";
                else
                    input_buffer = history[scroll_cursor];

                target_print(input_buffer.c_str());

                scrolling = true;
            }
            else if (rchar == 14) {
                scroll_cursor++;
                scroll_cursor %= MAX_HISTORY;
                clear_prompt();

                if (history[scroll_cursor] == nullptr)
                    input_buffer = "";
                else
                    input_buffer = history[scroll_cursor];

                target_print(input_buffer.c_str());

                scrolling = true;
            }
            else {
                input_buffer += rchar;
                if (target_terminal.echo)
                    Serial.print(rchar);
                scrolling = false;

            }
            old_char = rchar;
        }

        // Small delay to avoid excessive CPU usage
        delay(10);
    }
    return exit_code;
}
