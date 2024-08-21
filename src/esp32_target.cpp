#include "Arduino.h"

#include "se-target.h"
#include "se-sh.h"

int exit_code;
bool running;

void target_print(char* str) {
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
    static String inputLine;  // Static variable to retain value across loop iterations
    static bool new_iter = true;
    running = true;

    Serial.println();
    Serial.println("Welcome to se-sh!");

    while (running) {
        if (new_iter) {
            new_iter = false;
            inputLine = "";
            Serial.print("$ ");
        }
        if (Serial.available() > 0) {
            char receivedChar = Serial.read();

            if (receivedChar == '\n') {
                Serial.println(); // flush
                
                char mutableInput[inputLine.length() + 1];
                inputLine.toCharArray(mutableInput, inputLine.length() + 1);

                prompt(mutableInput);
                new_iter = true;
            }
            else if (receivedChar == '\r') {} // ignore 
            else {
                inputLine += receivedChar;
                Serial.print(receivedChar);

            }
        }

        // Small delay to avoid excessive CPU usage
        delay(10);
    }
    return exit_code;
}
