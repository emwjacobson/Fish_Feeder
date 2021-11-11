#ifndef SPI_H
#define SPI_H

#include <avr/io.h>

void SPI_MasterInit() {
    // Enable SPI, set as Master, 16div
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0); // 0101_0001
}

void SPI_Transmit(unsigned char data) {
    // Put data on the SPI Data Register
    SPDR = data;

    while (!(SPSR & (1 << SPIF))) { asm("nop"); };
}

#endif