#ifndef MENU_H
#define MENU_H

#include "nokia.h"

typedef enum ActionType { None, ChangeMenu } ActionType_t;

typedef struct row {
    char text[16];
    int action_type;
    void* action_data;
} row_t;

typedef struct menu {
    char title[16];
    unsigned char num_rows;
    row_t row[5];
    unsigned char selected_row;
} menu;

menu menus[] = {
    {
        "Menu          ",
        4,
        { 
            { "Time 1", ChangeMenu, &menus[1] },
            { "Time 2", ChangeMenu, &menus[2] },
            { "Time 3", ChangeMenu, &menus[3] },
            { "Set Time", None, (void *)0 }
        },
        1
    },
    {
        "Time 1        ",
        3,
        { 
            { "Back", ChangeMenu, &menus[0] },
            { "Up", None, (void *)0 },
            { "Down", None, (void *)0 },
        },
        1
    },
    {
        "Time 2        ",
        3,
        { 
            { "Back", ChangeMenu, &menus[0] },
            { "Up", None, (void *)0 },
            { "Down", None, (void *)0 },
        },
        1
    },
    {
        "Time 3        ",
        3,
        { 
            { "Back", ChangeMenu, &menus[0] },
            { "Up", None, (void *)0 },
            { "Down", None, (void *)0 },
        },
        1
    }
};

void Menu_DisplayMenu(menu* menu) {
    Screen_Clear();
    Screen_WriteString(menu->title, 1, 0);
    for(int i=1; i<=menu->num_rows; i++){
        Screen_SetCursor(0, i);
        Screen_WriteString(menu->row[i-1].text, 0, menu->selected_row == i ? 1 : 0);
    }
}

menu* Menu_GetMainMenu() {
    return &menus[0];
}

menu* Menu_Click(menu* cur) {
    switch ((*cur).row[cur->selected_row-1].action_type) {
        case None:
            break;
        case ChangeMenu:
            cur = (menu*)((*cur).row[cur->selected_row-1].action_data);
            cur->selected_row = 1;
            break;
    }
    return cur;
}

#endif