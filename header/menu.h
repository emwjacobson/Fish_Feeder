#ifndef MENU_H
#define MENU_H

#include <stdio.h>
#include <string.h>
#include "nokia.h"
#include "utils.h"
#include "times.h"

typedef enum HeaderType { HeaderNone, CurrentTime, TimerTime } HeaderType_t;
typedef enum ActionType { ActionNone, ChangeMenu, IncTimer, DecTimer } ActionType_t;
typedef enum LeaveAction { LeaveNone, LeaveSaveCurrent, LeaveSaveTimer } LeaveAction_t;

typedef struct row {
    char text[16];
    int action_type;
    void* action_data;
} row_t;

typedef struct menu {
    int header_type;
    unsigned char timer_id;
    int leave_action;
    char title[16];
    unsigned char num_rows;
    row_t row[5];
    unsigned char selected_row;
} menu_t;

menu_t menus[] = {
    {
        CurrentTime,
        0,
        LeaveNone,
        "     Menu     ",
        4,
        {
            { "Time 1", ChangeMenu, &menus[1] },
            { "Time 2", ChangeMenu, &menus[2] },
            { "Time 3", ChangeMenu, &menus[3] },
            { "Set Time", ActionNone, (void *)0 }
        },
        1
    },
    {
        TimerTime,
        1,
        LeaveSaveTimer,
        "    Time 1    ",
        3,
        { 
            { "      Up      ", IncTimer, (void *)0 },
            { "     Down     ", DecTimer, (void *)0 },
            { "     Back     ", ChangeMenu, &menus[0] },
        },
        1
    },
    {
        TimerTime,
        2,
        LeaveSaveTimer,
        "    Time 2    ",
        3,
        { 
            { "      Up      ", IncTimer, (void *)0 },
            { "     Down     ", DecTimer, (void *)0 },
            { "     Back     ", ChangeMenu, &menus[0] },
        },
        1
    },
    {
        TimerTime,
        3,
        LeaveSaveTimer,
        "    Time 3    ",
        3,
        { 
            { "      Up      ", IncTimer, (void *)0 },
            { "     Down     ", DecTimer, (void *)0 },
            { "     Back     ", ChangeMenu, &menus[0] },
        },
        1
    }
};

void Menu_DisplayMenu(menu_t* menu) {
    int i = 0;
    Screen_Clear();
    // Draw the header (if present)
    char header[16];
    const time_t* time;
    switch (menu->header_type) {
        case HeaderNone:
            break;
        case CurrentTime:
            time = Time_GetCurrentTime();
            sprintf(header, "%02d:%02d:%02d", time->hour, time->minute, time->second);
            Screen_SetCursor(35/2, i++);
            Screen_WriteString(header, 0, 0);
            break;
        case TimerTime:
            time = Time_GetTimer(menu->timer_id);
            sprintf(header, "     %02d:%02d    ", time->hour, time->minute);
            Screen_WriteString(header, 0, 0);
            i++;
            break;
    }

    // Draw the Menu
    if (strcmp(menu->title, "")) {
        Screen_SetCursor(0, i);
        Screen_WriteString(menu->title, 1, 0);
        i++;
    }
    for(int j = 1; j <= menu->num_rows; j++){
        Screen_SetCursor(0, i);
        Screen_WriteString(menu->row[j-1].text, 0, menu->selected_row == j ? 1 : 0);
        i++;
    }
}

menu_t* Menu_GetMainMenu() {
    return &menus[0];
}

menu_t* Menu_Click(menu_t* menu) {
    switch ((*menu).row[menu->selected_row-1].action_type) {
        case ActionNone:
            break;
        case ChangeMenu:
            // Do LeaveAction
            switch (menu->leave_action) {
                case LeaveNone:
                    // Do nothing
                    break;
                case LeaveSaveCurrent:
                case LeaveSaveTimer:
                    // Time_WriteEEPROM();
                    break;
            }
            
            menu = (menu_t*)((*menu).row[menu->selected_row-1].action_data);
            menu->selected_row = 1;

            // TODO: Do EnterAction
            break;
        case IncTimer:
            Time_IncrementTime(menu->timer_id);
            break;
        case DecTimer:
            Time_DecrementTime(menu->timer_id);
            break;
    }
    return menu;
}

#endif