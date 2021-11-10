#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <avr/io.h>
#include <avr/interrupt.h>

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerISR();

void TimerOn() {
    // Timer control register TCCR1
    TCCR1B = 0x0B;  // b3 = 0: CTC Mode (clear timer on compare)
                    // b2b1b0 = 011: pre-scaler /64
                    // 8MHz / 64 = 125,000 ticks/sec
                    // TCNT1 will count at 125,000 ticks/sec
    
    // Output compare register OCR1A
    OCR1A = 125;    // Timer interrupt will be generated when TCNT1 == OCR1A
                    // Want a 1ms tick. 0.001s * 125,000ticks/s = 125
                    // When TCNT == 125, then 1ms has passed

    // Timer interrupt mask register
    TIMSK1 = 0x02;  // bit1: OCIE1A - enabled compare match interrupt

    // Initialize counter
    TCNT1 = 0;

    _avr_timer_cntcurr = _avr_timer_M;

    // Enable global interrupts
    SREG |= 0x80;   // 0x80: 1000_0000
}

void TimerOff() {
    TCCR1B = 0x00; // bit3bit1bit0 = 000: timer off
}

ISR(TIMER1_COMPA_vect) {
    _avr_timer_cntcurr--;
    if (_avr_timer_cntcurr == 0) {
        TimerISR();
        _avr_timer_cntcurr = _avr_timer_M;
    }
}

void TimerSet(unsigned long M) {
    _avr_timer_M = M;
    _avr_timer_cntcurr = _avr_timer_M;
}

#endif