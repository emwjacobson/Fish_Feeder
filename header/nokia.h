#ifndef NOKIA_H
#define NOKIA_H

#include <avr/io.h>
#include "../header/utils.h"
#include "../header/spi.h"

void Screen_SendData(unsigned char type, unsigned char data);

// B0   UNUSED 
// B1   RESET
// B2   CE
// B3   DC
// B4   UNUSED
// B5   DIN (SPI MOSI)
// B6   UNUSED
// B7   CLK (SPI SCK)
#define SCN_RESET   1
#define SCN_CE      2
#define SCN_DC      3

#define SCN_DATA    1
#define SCN_COMMAND 0

void Screen_Init() {
    // Reset screen
    PORTB = SetBit(PORTB, SCN_RESET, 0);
    // for (int i=0; i<8000; i++) asm("nop");
    PORTB = SetBit(PORTB, SCN_RESET, 1);

    // Set some settings
    Screen_SendData(SCN_COMMAND, 0x23); // Set H=1
    Screen_SendData(SCN_COMMAND, 0x13); // Set Bias
    Screen_SendData(SCN_COMMAND, 0x22); // Set H=0
    Screen_SendData(SCN_COMMAND, 0x40); // Set Y=0
    Screen_SendData(SCN_COMMAND, 0x80); // Set Y=0
    Screen_SendData(SCN_COMMAND, 0x08); // Display Blank
    // Screen_SendData(SCN_COMMAND, 0x0C); // Normal Mode
    Screen_SendData(SCN_COMMAND, 0x0D); // Invert Mode
}

void Screen_Test() {
    Screen_SendData(SCN_DATA, 0x1F);
    Screen_SendData(SCN_DATA, 0x1F);
    Screen_SendData(SCN_DATA, 0x1F);
    Screen_SendData(SCN_DATA, 0x1F);
}

void Screen_SendData(unsigned char type, unsigned char data) {
    // Set it to either data or command mode
    PORTB = SetBit(PORTB, SCN_DC, type);

    // Enable clocking for screen
    PORTB = SetBit(PORTB, SCN_CE, 0);

    // Transmit data
    SPI_Transmit(data);

    // Disable clocking
    PORTB = SetBit(PORTB, SCN_CE, 1);
}

#endif