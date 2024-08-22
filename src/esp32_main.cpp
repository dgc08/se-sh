#include <Arduino.h>
#include <SPIFFS.h>

#include "se-target.h"
#include "esp_system.h"

#include "generic_arduino_target.h"

int shell_exit_code = 0;

void setup() {
    // Initialize serial communication at 115200 baud
    Serial.begin(115200);
    while (!Serial);
    Serial.println();

    while (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS Mount Failed, hanging up");
        delay(1000);
    }

    int code = target_shell();
    Serial.println();
    Serial.print("Shell exited with ");
    Serial.println(code);
    Serial.println();

    esp_restart();
}


void loop() {

}

void generic_arduino_reset() {
    esp_restart();
}

size_t target_write_file(const char* filename, Container contents) {
    File file = SPIFFS.open(filename, FILE_WRITE);
    if (!file) {
        return 0;
    }

    size_t w = file.write((uint8_t*)contents.content, contents.size);

    file.close();

    return w;
}


Container target_read_file(const char* filename) {
    File file = SPIFFS.open(filename, FILE_READ);
    if (!file) {
        return {0, 0};
    }

    size_t size = file.size();

    void* ret = malloc(size);
    if (ret == NULL) {
        file.close();
        return {0, 0};
    }

    size_t read = file.read((uint8_t*)ret, size);
    file.close();

    if (read != size) {
        free(ret);
        return {0, 0};
    }

    return Container{ret, size};
}

Container target_list_files() {
    File root = SPIFFS.open("/");
    if (!root || !root.isDirectory()) {
        return {0, 0};
    }

    size_t size = 1;
    File file = root.openNextFile();
    while (file) {
        size += strlen(file.name()) + 1; // File name length + '\n'
        file = root.openNextFile();
    }

    char* ret = (char*)malloc(size);
    if (ret == NULL) {
        return {0, 0};
    }

    ret[0] = '\0';
    root = SPIFFS.open("/");
    file = root.openNextFile();
    while (file) {
        strcat(ret, file.name());
        strcat(ret, "\n");
        file = root.openNextFile();
    }
    return {ret, size};
}

bool target_remove_file(const char* filename) {
    if (!SPIFFS.exists(filename))
        return false;

    return SPIFFS.remove(filename);
}
