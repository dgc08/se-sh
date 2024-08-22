#ifndef GENERIC_ARDUINO_TARGET_H_
#define GENERIC_ARDUINO_TARGET_H_

#include "esp32-hal-gpio.h"
#include <Arduino.h>

extern uint8_t pinModes[NUM_OUPUT_PINS];

int generic_arduino_pinMode (char* args);
int generic_arduino_digitalWrite(char* args);

#endif // GENERIC_ARDUINO_TARGET_H_
