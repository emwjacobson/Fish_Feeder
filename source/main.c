/*	Author: emerson
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include "../header/interrupt.h"
#include "../header/spi.h"
#include "../header/nokia.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

typedef struct task {
    int state;
    unsigned long period;
    unsigned long elapsedTime;
    int (*TickFct)(int);
} task;

#define NUM_TASKS 2
task tasks[NUM_TASKS];

volatile unsigned char TimerFlag = 0;
const unsigned long timerPeriod = 10;


unsigned char on = 0x00;
void TimerISR() {
    // unsigned char i;
    // for(i=0; i<NUM_TASKS; ++i) {
    //     if (tasks[i].elapsedTime >= tasks[i].period) {
    //         tasks[i].state = tasks[i].TickFct(tasks[i].state);
    //         tasks[i].elapsedTime = 0;
    //     }
    //     tasks[i].elapsedTime += timerPeriod;
    // }
    Screen_Test();
    PORTA = SetBit(PORTA, 0, 1);
}

// B0   UNUSED 
// B1   RESET
// B2   CE
// B3   DC
// B4   UNUSED
// B5   DIN (SPI MOSI)
// B6   UNUSED
// B7   CLK (SPI SCK)

int main(void) {
    // Outputs
    DDRB = 0xFF; PORTB = 0x00;
    DDRA = 0xFF; PORTA = 0x00;

    // Inputs
    // DDRA = 0x00; PORTA = 0xFF;
    // unsigned char i = 0;
    // tasks[i].state = ;
    // tasks[i].period = ;
    // tasks[i].elapsedTime = 0;
    // tasks[i].TickFct = &;

    // Enable SPI
    SPI_MasterInit();

    // Enable Nokia Screen
    Screen_Init();

    // Enable Interrupt for Task Scheduler
    TimerSet(1000);
    TimerOn();

    while (1) {}
    return 1;
}
