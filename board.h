/* Dominik Brzeziński */
#ifndef BOARD_HEADER
#define BOARD_HEADER
#include <ncurses.h>
#include <sys/time.h>

#define SETUP 0
#define STARTED 1
#define BOOM 2
#define WON 3

enum game_state { setup, started, boom, won };

typedef struct cell {
    int x, y;
    bool covered;
    bool mine;
    bool flag;
    int mines_around;
} cell_t;

typedef struct board {
    int width, height;
    int mines;
    struct cell *cells;
    int current_cell;
    int mines_set;
    int covered;
    int flags;
    double start_time;
    enum game_state game_state;
    bool draw_cursor;
} board_t;
void draw_board(const board_t *board, WINDOW *win);
board_t new_board(int width, int height, int mines);
void set_mines(board_t *board);
void uncover_cell(board_t *board, int cell);
void flood_uncover_cell(board_t *board, int cell);
#endif
