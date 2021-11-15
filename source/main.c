#define F_CPU 8000000

#include <avr/io.h>
#include "../header/interrupt.h"
#include "../header/spi.h"
#include "../header/nokia.h"
#include "../header/stepper.h"

typedef struct task {
    int state;
    unsigned long period;
    unsigned long elapsedTime;
    int (*TickFct)(int);
} task_t;

#define NUM_TASKS 3
task_t tasks[NUM_TASKS];

volatile unsigned char TimerFlag = 0;
const unsigned long timerPeriod = 1;

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

// ====== SHARED VARIABLES ======
unsigned char stepper_on = 0x00;
menu_t current_menu = { "Menu          ", 4, { "Row1", "Row2", "Row3", "Row4"}, 1 };
// ====== SHARED VARIABLES ======


// ====== Stepper Controller ======
//  Roles: Make the stepper step if the `stepper_on` variable resolves to True
//  Inputs: stepper_on
//  Outputs: 
// ====== Screen Controller ======
enum ST_States { ST_Start, ST_Main } ST_State;

int ST_Tick(int state) {
    // Transitions
    switch(state) {
        case ST_Start:
            state = ST_Main;
            break;
        case ST_Main:
            state = ST_Main;
            break;
    }
    
    // Actions
    switch(state) {
        case ST_Start:
            break;
        case ST_Main:
            if (stepper_on) {
                Stepper_Enable();
                Stepper_Step();
            } else {
                Stepper_Disable();
            }
            break;
    }

    return state;
}


// ====== Screen Controller ======
//  Roles: Display current menu from the `current_menu` variable
//  Inputs: current_menu
//  Outputs: 
// ====== Screen Controller ======
enum SC_States { SC_Start, SC_Display } SC_State;

int SC_Tick(int state) {
    // Transitions
    switch(state) {
        case SC_Start:
            state = SC_Display;
            break;
        case SC_Display:
            break;
    }

    // Actions
    switch(state) {
        case SC_Start:
            break;
        case SC_Display:
            Screen_DisplayMenu(current_menu);
            break;
    }
    return state;
}


// ====== Playground Machine ======
//  Roles: I just test stuff here
// ====== Playground Machine ======
enum PG_States { PG_Start, PG_IDLE, PG_SPIN } PG_State;

int PG_Tick(int state) {
    static unsigned char counter;
    unsigned char btn_up = GetBit(~PINA, 3);
    unsigned char btn_down = GetBit(~PINA, 4);
    // unsigned char btn_select = GetBit(~PINA, 5);
    switch(state) {
        case PG_Start:
            state = PG_IDLE;
            break;
        case PG_IDLE:
            if (btn_up) {
                Stepper_SetDirection(DIR_OUT);
                counter = 0;
                state = PG_SPIN;
            } else if (btn_down) {
                Stepper_SetDirection(DIR_IN);
                counter = 0;
                state = PG_SPIN;
            }
            break;
        case PG_SPIN:
            if (counter >= 50) {
                state = PG_IDLE;
            }
            break;
    }

    switch(state) {
        case PG_Start:
            break;
        case PG_IDLE:
            stepper_on = 0x00;
            break;
        case PG_SPIN:
            counter++;
            stepper_on = 0x01;
            break;
    }

    current_menu.selected_row++;
    if (current_menu.selected_row == current_menu.num_rows+1) current_menu.selected_row = 1;

    return state;
}

int main(void) {
    // Outputs
    DDRB = 0xFF; PORTB = 0x00;

    // Mixed
    // 0000_0111        1111_1000
    DDRA = 0x07; PORTA = 0xF8;

    // Setup all of the SMs
    unsigned char i = 0;
    tasks[i].state = ST_Start;
    tasks[i].period = 20;
    tasks[i].elapsedTime = 0;
    tasks[i].TickFct = &ST_Tick;
    i++;
    tasks[i].state = SC_Start;
    tasks[i].period = 500;
    tasks[i].elapsedTime = 0;
    tasks[i].TickFct = &SC_Tick;
    i++;
    tasks[i].state = PG_Start;
    tasks[i].period = 50;
    tasks[i].elapsedTime = 0;
    tasks[i].TickFct = &PG_Tick;

    // Enable SPI
    SPI_MasterInit();

    // Enable Nokia Screen
    Screen_Init();

    // Enable Stepper
    Stepper_Init();

    // Enable Interrupt for Task Scheduler
    TimerSet(timerPeriod);
    TimerOn();

    while (1) {}
    return 1;
}
