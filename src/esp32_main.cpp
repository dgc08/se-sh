#include <Arduino.h>
#include "se-target.h"
#include "esp_system.h"

int shell_exit_code = 0;

void setup() {
    // Initialize serial communication at 115200 baud
    Serial.begin(115200);
    while (!Serial);
    Serial.println();

    int code = target_shell();
    Serial.println();
    Serial.print("Shell exited with ");
    Serial.println(code);
    Serial.println();

    esp_restart();
}

void loop() {

}
