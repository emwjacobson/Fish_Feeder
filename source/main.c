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
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

int main(void) {
    // Outputs
    DDRB = 0xFF; PORTB = 0x00;

    // Inputs
    DDRA = 0x00; PORTA = 0xFF;

    /* Insert your solution below */
    while (1) {

    }
    return 1;
}
