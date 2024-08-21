#include "Arduino.h"

#include "se-target.h"
#include "se-sh.h"

int exit_code;
bool running;
Terminal target_terminal;

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

int target_shell() {
    target_terminal.echo = true;

    static String inputBuffer;
    static bool new_iter;
    static char old_char = 0;
    running = true;

    Serial.println("Welcome to se-sh!");
    new_iter = true;

    while (running) {
        if (new_iter) {
            new_iter = false;
            inputBuffer = "";
            if (target_terminal.echo)
                Serial.print("$ ");
        }
        if (Serial.available() > 0) {
            char rchar = Serial.read();

            if (rchar == '\r' || (rchar == '\n' && old_char != '\r')) {
                if (target_terminal.echo)
                    target_newline(); // flush
                
                char mutableInput[inputBuffer.length() + 1];
                inputBuffer.toCharArray(mutableInput, inputBuffer.length() + 1);

                int code = prompt(mutableInput);
                if (code < 0) {
                    return (code+1)*-1;
                }
                new_iter = true;
            }
            else if (rchar == '\n') {} // ignore
            else if (rchar == 0x8 || rchar == 0x7f) {
                if (inputBuffer.length())
                    target_print("\b \b");
                inputBuffer.remove(inputBuffer.length() - 1);
            }
            else if (rchar == 0x4) {
                target_newline();
                target_print("exit");
                if (target_terminal.echo)
                    Serial.print(rchar);
                return 0;
            }
            else {
                inputBuffer += rchar;
                if (target_terminal.echo)
                    Serial.print(rchar);

            }
            old_char = rchar;
        }

        // Small delay to avoid excessive CPU usage
        delay(10);
    }
    return exit_code;
}
