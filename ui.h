#ifndef UI_HEADER
#define UI_HEADER
#include <ncurses.h>
void draw_menu(WINDOW *win, int *width, int *height, int *mines,
               int *menu_state, int* preset, int c);

void draw_ui(WINDOW *win, double timer, int mines, int state);
#endif
