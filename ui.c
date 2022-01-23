#include "board.h"
#include <math.h>
#include <ncurses.h>

void draw_ui(WINDOW *win, double timer, int mines, int state) {
    char timer_str[128];
    werase(win);
    if (timer < 10) {
        sprintf(timer_str, "Time: %3.2f\tMines: %d", timer, mines);
    } else if (timer < 60) {
        sprintf(timer_str, "Time: %3.1f\tMines: %d", timer, mines);
    } else {
        sprintf(timer_str, "Time: %d:%02.0f\tMines: %d", (int)floor(timer / 60),
                fmod(timer, 60.), mines);
    }
    mvwaddstr(win, 0, 0, timer_str);
    if (state == WON) {
        mvwaddstr(win, 0, COLS / 2 - 4, "You have won!");
    } else if (state == BOOM) {
        mvwaddstr(win, 0, COLS / 2 - 4, "You have lost!");
    }
    wrefresh(win);
}

void draw_menu(WINDOW *win, int *width, int *height, int *mines,
               int *menu_state, int *preset, int c) {
    switch (c) {
    case KEY_LEFT:
    case 'h':
    case 'H':
        switch (*menu_state) {
        case 1:
            if (*width > 9) {
                (*width)--;
                *preset = 3;
            }
            break;
        case 2:
            if (*height > 9) {
                (*height)--;
                *preset = 3;
            }
            break;
        case 3:
            (*mines)--;
            *preset = 3;
            break;
        case 4:
            if (*preset > 0) {
                (*preset)--;
            } else {
                *preset = 3;
            }
            break;
        }
        break;
    case KEY_RIGHT:
    case 'l':
    case 'L':
        switch (*menu_state) {
        case 1:
            if (*width < COLS / 2) {
                (*width)++;
                *preset = 3;
            }
            break;
        case 2:
            if (*height < LINES - 1) {
                (*height)++;
                *preset = 3;
            }
            break;
        case 3:
            (*mines)++;
            *preset = 3;
            break;
        case 4:
            if (*preset < 3) {
                (*preset)++;
            } else {
                *preset = 0;
            }
            break;
        }
        break;
    case KEY_UP:
    case 'k':
    case 'K':
        if (*menu_state > 1) {
            (*menu_state)--;
        }
        break;
    case KEY_DOWN:
    case 'j':
    case 'J':
        if (*menu_state < 6) {
            (*menu_state)++;
        }
        break;
    }
    if (*preset == 3) {
        if (*mines > (*height - 1) * (*width - 1)) {
            *mines = (*height - 1) * (*width - 1);
        } else if (*mines < 1) {
            *mines = 1;
        }
    } else {
        int presets[3][3] = {{9, 9, 10}, {16, 16, 40}, {30, 16, 99}};
        *width = presets[*preset][0];
        *height = presets[*preset][1];
        *mines = presets[*preset][2];
    }

    box(win, 0, 0);
    char width_str[17], height_str[17], mines_str[17];
    sprintf(width_str, "Width: %9d", *width);
    sprintf(height_str, "Height: %8d", *height);
    sprintf(mines_str, "Mines: %9d", *mines);

    mvwaddstr(win, 0, 5, "New Game");
    if (*menu_state == 1) {
        wattron(win, A_REVERSE);
    }
    mvwaddstr(win, 1, 1, width_str);
    wattroff(win, A_REVERSE);

    if (*menu_state == 2) {
        wattron(win, A_REVERSE);
    }
    mvwaddstr(win, 2, 1, height_str);
    wattroff(win, A_REVERSE);

    if (*menu_state == 3) {
        wattron(win, A_REVERSE);
    }
    mvwaddstr(win, 3, 1, mines_str);
    wattroff(win, A_REVERSE);
    if (*menu_state == 4) {
        wattron(win, A_REVERSE);
    }
    switch (*preset) {
    case 0:
        mvwaddstr(win, 4, 1, "      EASY      ");
        break;
    case 1:
        mvwaddstr(win, 4, 1, "  INTERMEDIATE  ");
        break;
    case 2:
        mvwaddstr(win, 4, 1, "     EXPERT     ");
        break;
    case 3:
        mvwaddstr(win, 4, 1, "     CUSTOM     ");
        break;
    }
    wattroff(win, A_REVERSE);
    if (*menu_state == 5) {
        wattron(win, A_REVERSE);
    }
    mvwaddstr(win, 5, 1, "       OK       ");
    wattroff(win, A_REVERSE);

    if (*menu_state == 6) {
        wattron(win, A_REVERSE);
    }
    mvwaddstr(win, 6, 1, "     Cancel     ");
    wattroff(win, A_REVERSE);
    wrefresh(win);
}
