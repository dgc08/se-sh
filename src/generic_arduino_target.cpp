#include "Arduino.h"

#include "generic_arduino_target.h"
#include "se-target.h"
#include "se-sh.h"
#include "s_utils.h"

#include "sys/_stdint.h"
#include <string.h>

#define MAX_HISTORY 16

uint8_t pinModes[NUM_OUPUT_PINS];

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
    char* com = get_arg(&command);

    if (strcmp(com, "pinMode") == 0) {
        return generic_arduino_pinMode(command);
    }
    else if (strcmp(com, "digitalWrite") == 0) {
        return generic_arduino_digitalWrite(command);
    }
    else if (strcmp(com, "reset") == 0) {
        generic_arduino_reset();
        target_print("Reset failed."); // You shall never reach this line
        return 1;
    }

    target_print("sys command \"");
    target_print(com);
    target_print("\" not found.");
    target_newline();
    return 1;
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

int target_prompt(String buf) {
    char mutableInput[input_buffer.length() + 1];
    input_buffer.toCharArray(mutableInput, input_buffer.length() + 1);

    return prompt(mutableInput);
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
                
                int code = target_prompt(input_buffer);
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

// //GPIO FUNCTIONS
// #define INPUT             0x01
// // Changed OUTPUT from 0x02 to behave the same as Arduino pinMode(pin,OUTPUT)
// // where you can read the state of pin even when it is set as OUTPUT
// #define OUTPUT            0x03
// #define PULLUP            0x04
// #define INPUT_PULLUP      0x05
// #define PULLDOWN          0x08
// #define INPUT_PULLDOWN    0x09
// #define OPEN_DRAIN        0x10
// #define OUTPUT_OPEN_DRAIN 0x13
// #define ANALOG            0xC0

int generic_arduino_pinMode_API(uint8_t pin, uint8_t mode) {
    if (!GPIO_IS_VALID_GPIO(pin)) {
        target_print("Invalid pin selected");
        target_newline();
        return 1;
    }

    if (pin < NUM_OUPUT_PINS)
        pinModes[pin] = mode;
    pinMode(pin, mode);

    return 0;
}

int generic_arduino_pinMode (char* args) {
    uint8_t pin = atoi(get_arg(&args));
    uint8_t mode = atoi(args);

    generic_arduino_pinMode_API(pin, mode);

    target_print("pinMode for pin ");
    target_print_int(pin);
    target_print(" set to ");
    target_print_int(mode);
    target_newline();

    return 0;
}

int generic_arduino_digitalWrite(char* args) {
    uint8_t pin = atoi(get_arg(&args));
    uint8_t val = atoi(args);

    if (!GPIO_IS_VALID_GPIO(pin)) {
        target_print("Invalid pin selected");
        target_newline();
        return 1;
    }
    if (pin < NUM_OUPUT_PINS && pinModes[pin] == 0) {
        target_print("Setting pinMode to OUTPUT...");
        generic_arduino_pinMode_API(pin, OUTPUT);
        target_print(" Set to ");
        target_print_int(pinModes[pin]);
        target_newline();
    }

    target_print("digitalWrite for pin ");
    target_print_int(pin);
    target_print(" set to ");
    target_print_int(val);

    digitalWrite(pin, val);

    target_newline();

    return 0;
}
