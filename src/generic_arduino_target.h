#ifndef GENERIC_ARDUINO_TARGET_H_
#define GENERIC_ARDUINO_TARGET_H_

#include <Arduino.h>

extern uint8_t pinModes[NUM_OUPUT_PINS];

void generic_arduino_reset();

int generic_arduino_pinMode(char* args);
int generic_arduino_digitalWrite(char* args);


void IRAM_ATTR generic_arduino_check_exit_key();

extern String input_buffer;

#endif // GENERIC_ARDUINO_TARGET_H_
