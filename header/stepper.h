#ifndef STEPPER_H
#define STEPPER_H

#include <avr/io.h>
#include <util/delay.h>
#include "../header/utils.h"

#define DIR_OUT 0
#define DIR_IN 1

// PORTA = SetBit(PORTA, 0, 0);
// PORTA = SetBit(PORTA, 1, 1);
// PORTA = SetBit(PORTA, 1, 0);

void Stepper_Init() {
    PORTA = SetBit(PORTA, 0, DIR_OUT);
}

void Stepper_Step() {
    PORTA = SetBit(PORTA, 1, 1);
    _delay_us(10);
    PORTA = SetBit(PORTA, 1, 0);
}

// 1 = DIR_IN
// 0 = DIR_OUT
void Stepper_SetDirection(int dir) {
    PORTA = SetBit(PORTA , 0, dir);
}

#endif