#ifndef ADC_H
#define ADC_H

#include <avr/io.h>
#include "utils.h"

#define ADMUX_REF 0x00

void ADC_Init() {
    // Set default to ADC Channel 3
    ADMUX = 0x03;
    // Enable ADC and take first reading
    ADCSRA = 0xC0;

    while (ADCSRA & 0x40) { asm("nop"); };
}

unsigned short ADC_ReadChannel(const unsigned char channel) {
    unsigned short reading;
    // Set channel to be read
    ADMUX = ADMUX_REF | (channel & 0x07);
    // Take reading
    ADCSRA = SetBit(ADCSRA, 6, 1);
    // Wait for reading to finish
    while (ADCSRA & 0x40) { asm("nop"); };
    reading = ADCL | ((ADCH & 0x03) << 8);

    return reading;
}

#endif