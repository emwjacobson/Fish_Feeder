#define F_CPU 8000000

#include <stdlib.h>
#include <avr/io.h>
#include <stdio.h>
#include "../header/utils.h"
#include "../header/interrupt.h"
#include "../header/spi.h"
#include "../header/nokia.h"
#include "../header/stepper.h"
#include "../header/adc.h"
#include "../header/menu.h"
#include "../header/times.h"

#define JOYSTICK_DEADZONE 400

typedef struct task {
    int state;
    unsigned long period;
    unsigned long elapsedTime;
    int (*TickFct)(int);
} task_t;

#define NUM_TASKS 5
task_t tasks[NUM_TASKS];

// volatile unsigned char TimerFlag = 0;
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
// unsigned char stepper_on = 0x00;
unsigned char btn_up = 0x00;
unsigned char btn_down = 0x00;
unsigned char btn_left = 0x00;
unsigned char btn_right = 0x00;
unsigned char btn_select = 0x00;
menu_t* current_menu;
unsigned char num_steps = 0;
// ====== SHARED VARIABLES ======


// ====== Time Machine ======
//  Roles: This machine keeps track of the time of the system and coordinates time sensitive jobs
//  Inputs: 
//  Outputs: 
// ====== Time Machine ======
enum Time_States { Time_Start, Time_Spin } Time_State;

int Time_Tick(int state) {
    time_t* time;
    time_t* cur_time;

    // Transitions
    switch(state) {
        case Time_Start:
            state = Time_Spin;
            break;
        case Time_Spin:
            state = Time_Spin;
            break;
    }

    // Actions
    switch(state) {
        case Time_Start:
            break;
        case Time_Spin:
            Time_IncSecond();
            cur_time = Time_GetCurrentTime();
            for (unsigned char i=0; i<NUM_TIMERS; i++) {
                time = Time_GetTimer(i);
                // If the time is the same, then run the stepper
                if (cur_time->hour == time->hour && cur_time->minute == time->minute && cur_time->second == 0) {
                    num_steps = 100;
                }
            }
            break;
    }
    return state;
}


// ====== Stepper Controller ======
//  Roles: Make the stepper step if the `stepper_on` variable resolves to True
//  Inputs: stepper_on
//  Outputs: 
// ====== Screen Controller ======
enum Stepper_States { Stepper_Start, Stepper_Main } Stepper_State;

int Stepper_Tick(int state) {
    // Transitions
    switch(state) {
        case Stepper_Start:
            state = Stepper_Main;
            break;
        case Stepper_Main:
            state = Stepper_Main;
            break;
    }
    
    // Actions
    switch(state) {
        case Stepper_Start:
            break;
        case Stepper_Main:
            if (num_steps > 0) {
                num_steps--;
                // Could honestly use PWM for this, but brings a level of complexity that I don't have time for
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
enum Screen_States { Screen_Start, Screen_Display } Screen_State;

int Screen_Tick(int state) {
    // Transitions
    switch(state) {
        case Screen_Start:
            state = Screen_Display;
            break;
        case Screen_Display:
            break;
    }

    // Actions
    switch(state) {
        case Screen_Start:
            break;
        case Screen_Display:
            Menu_DisplayMenu(current_menu);
            break;
    }

    return state;
}


// ====== Joystick Machine ======
//  Roles: Read values from the joystick (X, Y, and Btn)
//  Inputs: 
//  Outputs: btn_up, btn_down, btn_left, btn_right, btn_select
// ====== Joystick Machine ======
enum Joystick_States { Joystick_Start, Joystick_Read } Joystick_State;

int Joystick_Tick(int state) {
    unsigned short x = ADC_ReadChannel(3);
    unsigned short y = ADC_ReadChannel(4);
    unsigned char btn = GetBit(~PINA, 5);

    // Traitions
    switch(state) {
        case Joystick_Start:
            state = Joystick_Read;
            break;
        case Joystick_Read:
            state = Joystick_Read;
            break;
    }

    // Actions
    switch(state) {
        case Joystick_Start:
            break;
        case Joystick_Read:
            btn_select = btn;
            
            btn_down = (x > 512 + JOYSTICK_DEADZONE);
            btn_up = (x < 512 - JOYSTICK_DEADZONE);
            btn_left = (y > 512 + JOYSTICK_DEADZONE);
            btn_right = (y < 512 - JOYSTICK_DEADZONE);
            break;
    }
    return state;
}

// ====== Menu Machine ======
//  Roles: This is how we interact with the menu using the outputs from the Joystick SM
//  Inputs: btn_up btn_down btn_left btn_right btn_select
//  Outputs: 
// ====== Menu Machine ======
enum M_States { M_Start, M_Idle, M_Left, M_Right, M_Up, M_Down, M_Click } M_State;

int M_Tick(int state) {
    // Transition
    switch(state) {
        case M_Start:
            state = M_Idle;
            break;
        case M_Idle:
            if (btn_select) {
                current_menu = Menu_Click(current_menu);
                state = M_Click;
            } else if (btn_left && !btn_right && !btn_up && !btn_down) {
                state = M_Left;
            } else if (!btn_left && btn_right && !btn_up && !btn_down) {
                state = M_Right;
            } else if (!btn_left && !btn_right && btn_up && !btn_down) {

                (*current_menu).selected_row--;
                if ((*current_menu).selected_row == 0) (*current_menu).selected_row = (*current_menu).num_rows;

                state = M_Up;
            } else if (!btn_left && !btn_right && !btn_up && btn_down) {

                (*current_menu).selected_row++;
                if ((*current_menu).selected_row == (*current_menu).num_rows+1) (*current_menu).selected_row = 1;

                state = M_Down;
            }
            break;
        case M_Left:
            state = btn_left ? M_Left : M_Idle;
            break;
        case M_Right:
            state = btn_right ? M_Right : M_Idle;
            break;
        case M_Up:
            state = btn_up ? M_Up : M_Idle;
            break;
        case M_Down:
            state = btn_down ? M_Down : M_Idle;
            break;
        case M_Click:
            state = btn_select ? M_Click : M_Idle;
            break;
    }

    // Actions
    switch(state) {
        case M_Start:
            state = M_Idle;
            break;
        case M_Idle:
            break;
        case M_Left:
            break;
        case M_Right:
            break;
        case M_Up:
            break;
        case M_Down:
            break;
        case M_Click:
            break;
    }

    return state;
}

int main(void) {
    // Outputs
    DDRB = 0xFF; PORTB = 0x00;

    // Mixed
    // 0000_0111        1110_0000
    DDRA = 0x07; PORTA = 0xD0;

    // Disable digital pins where analog pins are enabled
    DIDR0 = (1 << ADC3D) | 1 << (ADC4D);

    // Setup all of the SMs
    unsigned char i = 0;
    tasks[i].state = Time_Start;
    tasks[i].period = 10;
    tasks[i].elapsedTime = 0;
    tasks[i].TickFct = &Time_Tick;
    i++;
    tasks[i].state = Stepper_Start;
    tasks[i].period = 20;
    tasks[i].elapsedTime = 0;
    tasks[i].TickFct = &Stepper_Tick;
    i++;
    tasks[i].state = Screen_Start;
    tasks[i].period = 100;
    tasks[i].elapsedTime = 0;
    tasks[i].TickFct = &Screen_Tick;
    i++;
    tasks[i].state = Joystick_Start;
    tasks[i].period = 50;
    tasks[i].elapsedTime = 0;
    tasks[i].TickFct = &Joystick_Tick;
    i++;
    tasks[i].state = M_Start;
    tasks[i].period = 50;
    tasks[i].elapsedTime = 0;
    tasks[i].TickFct = &M_Tick;

    // Enable SPI
    SPI_MasterInit();

    // Enable Nokia Screen
    Screen_Init();

    // Enable Stepper
    Stepper_Init();

    // Enable ADC
    ADC_Init();

    // Enable Time Module
    Time_Init();

    // Screen_WriteString(" Feesh Feeder ", 1, 0);
    // delay_ms(500);

    current_menu = Menu_GetMainMenu();

    // Enable Interrupt for Task Scheduler
    TimerSet(timerPeriod);
    TimerOn();

    while (1) {}
    return 1;
}