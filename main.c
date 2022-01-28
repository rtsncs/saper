/* Dominik Brzeziński */
#include "board.h"
#include "colors.h"
#include "input.h"
#include "ui.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#define REFRESH_RATE 60

void init() {
    initscr();
    cbreak();
    noecho();
    nl();
    keypad(stdscr, TRUE);
    curs_set(0);
    nodelay(stdscr, true);
    init_colors();

    mousemask(BUTTON1_RELEASED | BUTTON2_RELEASED | BUTTON3_RELEASED, NULL);
    mouseinterval(10);
}

int main(int argc, char **argv) {
    if (argc > 1 &&
        (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
        printf("\
Saper\n\
Sterowanie:\n\
\tporuszanie się - hjkl lub strzałki\n\
\todkrywanie pól - enter lub lewy przycisk myszy\n\
\todkrywanie pól dookoła - spacja lub środkowy przycisk myszy\n\
\tstawianie flag - f lub prawy przycisk myszy\n\
\tn - nowa gra\n\
\tq - wyjście\n");
        return 0;
    }
    init();

    int presets[3][3] = {{9, 9, 10}, {16, 16, 40}, {30, 16, 99}};
    int cols = COLS;
    int lines = LINES;
    int preset = 1;
    int width = presets[preset][0];
    int height = presets[preset][1];
    int mines = presets[preset][2];

    WINDOW *game_win = newwin(height, width * 2, (lines / 2) - (height / 2),
                              (cols / 2) - (width));
    WINDOW *ui_win = newwin(0, 0, lines - 1, 0);
    WINDOW *menu_win = newwin(8, 18, (lines / 2) - 4, (cols / 2) - 9);
    board_t board = new_board(width, height, mines);
    int c;
    int menu_state = 0; /* 0 - hidden 1 - width 2 - height 3 - mines 4 preset 5
                           - ok 6 - cancel*/
    double timer = 0;
    struct timeval now;
    struct timeval wait;
    wait.tv_sec = 0;
    wait.tv_usec = 1000000 / REFRESH_RATE;
    wattron(ui_win, A_REVERSE);
    srand(time(NULL));
    while (true) {
        c = getch();
        if (c == 'q' || c == 'Q') {
            break;
        }

        if ((c == 'n' || c == 'N') && !menu_state) {
            if (board.game_state == WON || board.game_state == BOOM) {
                menu_state = 5;
            } else {
                menu_state = 6;
            }
        }

        if (lines != LINES || cols != COLS) {
            lines = LINES;
            cols = COLS;
            mvwin(game_win, (lines / 2) - (height / 2), (cols / 2) - width);
            mvwin(ui_win, lines - 1, 0);
            mvwin(menu_win, (lines / 2) - 9, (cols / 2) - 2);
            erase();
        }
        if (menu_state) {
            if ((menu_state == 5 || menu_state == 6) && c == '\n') {
                if (menu_state == 5) {
                    if (preset != 3) {
                        width = presets[preset][0];
                        height = presets[preset][1];
                        mines = presets[preset][2];
                    }
                    timer = 0;
                    free(board.cells);
                    board = new_board(width, height, mines);
                    wresize(game_win, height, width * 2);
                    mvwin(game_win, (lines / 2) - (height / 2),
                          (cols / 2) - width);
                    erase();
                } else {
                    width = board.width;
                    height = board.height;
                    mines = board.mines;
                }
                menu_state = 0;
            } else {
                draw_menu(menu_win, &width, &height, &mines, &menu_state,
                          &preset, c);
            }
        } else {
            if (board.game_state == STARTED) {
                gettimeofday(&now, NULL);
                double now_ms = now.tv_sec + (now.tv_usec / 1000000.);
                timer = now_ms - board.start_time;
            }
            if ((board.game_state != WON && board.game_state != BOOM)) {
                handle_input(c, &board, game_win);
                if (board.mines == board.covered)
                    board.game_state = WON;
            }
            draw_board(&board, game_win);
            int mines = board.mines - board.flags;
            draw_ui(ui_win, timer, mines, board.game_state);
        }
        select(0, NULL, NULL, NULL, &wait);
    }
    free(board.cells);
    delwin(game_win);
    delwin(ui_win);
    delwin(menu_win);
    endwin();
    return 0;
}
