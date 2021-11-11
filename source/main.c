#include <avr/io.h>
#include "../header/interrupt.h"
#include "../header/spi.h"
#include "../header/nokia.h"

typedef struct task {
    int state;
    unsigned long period;
    unsigned long elapsedTime;
    int (*TickFct)(int);
} task_t;

#define NUM_TASKS 1
task_t tasks[NUM_TASKS];

volatile unsigned char TimerFlag = 0;
const unsigned long timerPeriod = 10;

void TimerISR() {
    unsigned char i;
    for(i=0; i<NUM_TASKS; ++i) {
        if (tasks[i].elapsedTime >= tasks[i].period) {
            tasks[i].state = tasks[i].TickFct(tasks[i].state);
            tasks[i].elapsedTime = 0;
        }
        tasks[i].elapsedTime += timerPeriod;
    }
}

menu_t current_menu = { "Menu          ", 4, { "Row1", "Row2", "Row3", "Row4"}, 1 };

enum PG_States { PG_Start, PG_StepON } PG_State;

int PG_Tick(int state) {
    // Transition
    switch(state) {
        case PG_Start:
            state = PG_StepON;
            PORTA = SetBit(PORTA, 0, 0);
            break;
        case PG_StepON:
            state = PG_StepON;
            break;
    }

    // Actions
    switch(state) {
        case PG_Start:
            break;
        case PG_StepON:
            PORTA = SetBit(PORTA, 1, 1);
            PORTA = SetBit(PORTA, 1, 0);
            // Screen_DisplayMenu(current_menu);
            // current_menu.selected_row = current_menu.selected_row + 1;
            // if (current_menu.selected_row == current_menu.num_rows + 1) current_menu.selected_row = 1;
            break;
    }
    return state;
}

int main(void) {
    // Outputs
    DDRB = 0xFF; PORTB = 0x00;
    DDRA = 0xFF; PORTA = 0x00;

    // Inputs
    // DDRA = 0x00; PORTA = 0xFF;

    unsigned char i = 0;
    tasks[i].state = PG_Start;
    tasks[i].period = 10;
    tasks[i].elapsedTime = 0;
    tasks[i].TickFct = &PG_Tick;

    // Enable SPI
    SPI_MasterInit();

    // Enable Nokia Screen
    Screen_Init();

    // Enable Interrupt for Task Scheduler
    TimerSet(timerPeriod);
    TimerOn();

    while (1) {}
    return 1;
}
