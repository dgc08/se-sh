#include <Arduino.h>
#include "se-target.h"

int shell_exit_code = 0;

void setup() {
    // Initialize serial communication at 115200 baud
    Serial.begin(115200);
    Serial.println("Enter a line of text:");
}

void loop() {
    int code = target_shell();
    Serial.print("\nShell exited with ");
    Serial.println(code);
}
