/* Dominik Brzeziński */
#include "board.h"
#include <ncurses.h>

void handle_input(int c, board_t *board, WINDOW *win) {
    MEVENT event;
    int new_cell;
    if (c == KEY_MOUSE && getmouse(&event) == OK) {
        int x;
        int y;

        getbegyx(win, y, x);
        x = (event.x - x) / 2;
        y = event.y - y;

        if (x >= 0 && x < board->width && y >= 0 && y < board->height) {
            board->current_cell = x * board->height + y;
            board->draw_cursor = false;
            if (event.bstate & BUTTON1_RELEASED) {
                c = '\n';
            } else if (event.bstate & BUTTON3_RELEASED) {
                c = 'f';
            } else if (event.bstate & BUTTON2_RELEASED) {
                c = ' ';
            }
        }
    }
    if (board->game_state != WON) {
        switch (c) {
        case 'h':
        case 'H':
        case KEY_LEFT:
            new_cell = board->current_cell - board->height;
            if (new_cell >= 0) {
                board->current_cell -= board->height;
            }
            board->draw_cursor = true;
            break;
        case 'l':
        case 'L':
        case KEY_RIGHT:
            new_cell = board->current_cell + board->height;
            if (new_cell < board->width * board->height) {
                board->current_cell += board->height;
            }
            board->draw_cursor = true;
            break;
        case 'j':
        case 'J':
        case KEY_DOWN:
            if ((board->current_cell + 1) % board->height != 0) {
                board->current_cell++;
            }
            board->draw_cursor = true;
            break;
        case 'k':
        case 'K':
        case KEY_UP:
            if (board->current_cell % board->height != 0) {
                board->current_cell--;
            }
            board->draw_cursor = true;
            break;
        case '\n':
            if (board->game_state == SETUP) {
                set_mines(board);
                struct timeval tv;
                gettimeofday(&tv, NULL);
                board->start_time = tv.tv_sec + (tv.tv_usec / 1000000.);
                board->game_state = STARTED;
            }
            uncover_cell(board, board->current_cell);
            break;
        case ' ':
            if (board->game_state == STARTED) {
                flood_uncover_cell(board, board->current_cell);
            }
            break;
        case 'f':
        case 'F':
            if (board->cells[board->current_cell].covered) {
                if (board->cells[board->current_cell].flag) {
                    board->flags--;
                } else {
                    board->flags++;
                }
                board->cells[board->current_cell].flag =
                    !board->cells[board->current_cell].flag;
            }
            break;
        }
    }
}
