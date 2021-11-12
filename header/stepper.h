#ifndef STEPPER_H
#define STEPPER_H

#include <avr/io.h>
#include <util/delay.h>
#include "../header/utils.h"

#define ST_PORT PORTA
#define ST_DIR_PIN 0
#define ST_STEP_PIN 1

#define DIR_OUT 0
#define DIR_IN 1

void Stepper_Init() {
    ST_PORT = SetBit(ST_PORT, ST_DIR_PIN, DIR_OUT);
}

void Stepper_Step() {
    // Toggle the bit to 1
    ST_PORT = SetBit(ST_PORT, ST_STEP_PIN, 1);
    ST_PORT = SetBit(ST_PORT, ST_STEP_PIN, 0);
}

// 1 = DIR_IN
// 0 = DIR_OUT
void Stepper_SetDirection(int dir) {
    ST_PORT = SetBit(ST_PORT , ST_DIR_PIN, dir);
}

#endif