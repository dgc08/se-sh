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

int target_shell() {
    target_terminal.echo = true;

    static String inputLine;
    static bool new_iter;
    running = true;

    Serial.println();
    Serial.println("Welcome to se-sh!");
    new_iter = true;

    while (running) {
        if (new_iter) {
            new_iter = false;
            inputLine = "";
            if (target_terminal.echo)
                Serial.print("$ ");
        }
        if (Serial.available() > 0) {
            char receivedChar = Serial.read();

            if (receivedChar == '\n') {
                if (target_terminal.echo)
                    Serial.println(); // flush
                
                char mutableInput[inputLine.length() + 1];
                inputLine.toCharArray(mutableInput, inputLine.length() + 1);

                int code = prompt(mutableInput);
                if (code < 0) {
                    return (code+1)*-1;
                }
                new_iter = true;
            }
            else if (receivedChar == '\r') {} // ignore
            else if (receivedChar == 0x4) {
                target_print("\nexit");
                return 0;
            }
            else {
                inputLine += receivedChar;
                if (target_terminal.echo)
                    Serial.print(receivedChar);

            }
        }

        // Small delay to avoid excessive CPU usage
        delay(10);
    }
    return exit_code;
}
