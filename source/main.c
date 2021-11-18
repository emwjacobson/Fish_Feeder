#define F_CPU 8000000

#include <avr/io.h>
#include "../header/interrupt.h"
#include "../header/spi.h"
#include "../header/nokia.h"
#include "../header/stepper.h"
#include "../header/adc.h"

#define JOYSTICK_DEADZONE 250

typedef struct task {
    int state;
    unsigned long period;
    unsigned long elapsedTime;
    int (*TickFct)(int);
} task_t;

#define NUM_TASKS 4
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
unsigned char btn_up = 0x00;
unsigned char btn_down = 0x00;
unsigned char btn_left = 0x00;
unsigned char btn_right = 0x00;
unsigned char btn_select = 0x00;
menu_t current_menu = { "Menu          ", 4, { "Row1", "Row2", "Row3", "Row4"}, 1 };
// ====== SHARED VARIABLES ======


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
            Screen_DisplayMenu(current_menu);
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
            
            btn_up = (x > 512 + JOYSTICK_DEADZONE);
            btn_down = (x < 512 - JOYSTICK_DEADZONE);
            btn_left = (y > 512 + JOYSTICK_DEADZONE);
            btn_right = (y < 512 - JOYSTICK_DEADZONE);
            break;
    }
    return state;
}

// ====== Playground Machine ======
//  Roles: I just test stuff here
// ====== Playground Machine ======
enum PG_States { Playground_Start, Playground_Idle, Playground_Spin } Playground_State;

int Playground_Tick(int state) {
    switch(state) {
        case Playground_Start:
            state = Playground_Idle;
            break;
        case Playground_Idle:
            break;
        case Playground_Spin:
            break;
    }

    switch(state) {
        case Playground_Start:
            break;
        case Playground_Idle:
            break;
        case Playground_Spin:
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
    tasks[i].state = Playground_Start;
    tasks[i].period = 50;
    tasks[i].elapsedTime = 0;
    tasks[i].TickFct = &Playground_Tick;

    // Enable SPI
    SPI_MasterInit();

    // Enable Nokia Screen
    Screen_Init();

    // Enable Stepper
    Stepper_Init();

    // Enable ADC
    ADC_Init();

    Screen_WriteString("Pogg", 0, 0);
    delay_ms(1000);

    // Enable Interrupt for Task Scheduler
    TimerSet(timerPeriod);
    TimerOn();

    while (1) {}
    return 1;
}
