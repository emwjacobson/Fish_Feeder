#ifndef TIMES_H
#define TIMES_H

#include <avr/io.h>
#include <avr/eeprom.h>

#define NUM_TIMERS 3
#define TIME_OFFSET (void*)0x0E
#define TIMERS_OFFSET (void*)0x20

typedef struct time {
    char hour;
    char minute;
    char second;
} time_t;

time_t current_time;
time_t timers[NUM_TIMERS];

void Time_Init() {
    while (!eeprom_is_ready()) { asm("nop"); };
    eeprom_read_block(&current_time, TIME_OFFSET, sizeof(time_t));
    eeprom_read_block(&timers, TIMERS_OFFSET, sizeof(time_t)*NUM_TIMERS);

    if (current_time.hour < 0) current_time.hour = 0;
    if (current_time.minute < 0) current_time.minute = 0;
    if (current_time.second < 0) current_time.second = 0;

    for(int i=0; i<NUM_TIMERS; i++) {
        if (timers[i].hour < 0) timers[i].hour = 0;
        if (timers[i].minute < 0) timers[i].minute = 0;
        if (timers[i].second < 0) timers[i].second = 0;
    }
}

void Time_WriteEEPROM() {
    eeprom_write_block(&current_time, TIME_OFFSET, sizeof(time_t));
    eeprom_write_block(&timers, TIMERS_OFFSET, sizeof(time_t)*NUM_TIMERS);
}

void Time_IncSecond() {
    if (current_time.second + 1 >= 60) {
        current_time.second = 0;
        if (current_time.minute + 1 >= 60) {
            current_time.minute = 0;
            if (current_time.hour + 1 >= 25) {
                current_time.hour = 0;
            } else {
                current_time.hour++;
            }
        } else {
            current_time.minute++;
        }
        // Save the time every minute
        Time_WriteEEPROM();
    } else {
        current_time.second++;
    }
}

const time_t* Time_GetCurrentTime() {
    return &current_time;
}

// The 1-based index of timers
time_t* Time_GetTimer(const unsigned char num) {
    if (num == 0) return (void*)0;
    return &timers[num-1];
}

void Time_IncrementTime(const unsigned char num) {
    timers[num-1].minute += 30;
    if (timers[num-1].minute >= 60) {
        timers[num-1].minute = timers[num-1].minute % 60;
        timers[num-1].hour++;
        if (timers[num-1].hour >= 24) {
            timers[num-1].hour = 0;
        }
    }
}

void Time_DecrementTime(const unsigned char num) {
    timers[num-1].minute -= 30;
    if (timers[num-1].minute < 0) {
        timers[num-1].minute = 30;
        timers[num-1].hour--;
        if (timers[num-1].hour < 0) {
            timers[num-1].hour = 23;
        }
    }
}

#endif