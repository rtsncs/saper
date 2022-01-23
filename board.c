#include "board.h"
#include <ncurses.h>
#include <stdlib.h>

void draw_board(const board_t *board, WINDOW *win) {
    int i;
    wattron(win, A_BOLD);
    for (i = 0; i < board->width * board->height; i++) {
        if (board->current_cell == i && board->draw_cursor) {
            wattron(win, A_REVERSE);
        }
        const cell_t *cell = (board->cells) + i;
        if (cell->flag) {
            wattron(win, A_UNDERLINE);
            if (board->game_state == BOOM && !cell->mine) {
                wattron(win, COLOR_PAIR(6));
                wattron(win, A_REVERSE);
            } else {
                wattron(win, COLOR_PAIR(5));
            }
            mvwaddch(win, cell->y, cell->x * 2, 'F');
            wattroff(win, A_UNDERLINE);
            wattroff(win, COLOR_PAIR(5));
            wattroff(win, COLOR_PAIR(6));
            wattroff(win, A_REVERSE);
        } else if (cell->covered) {
            wattroff(win, A_BOLD);
            mvwaddch(win, cell->y, cell->x * 2, '~');
            wattron(win, A_BOLD);
        } else if (cell->mine) {
            wattron(win, COLOR_PAIR(3));
            mvwaddch(win, cell->y, cell->x * 2, 'X');
            wattroff(win, COLOR_PAIR(3));
        } else if (cell->mines_around) {
            int count = cell->mines_around;
            wattron(win, COLOR_PAIR(count));
            mvwaddch(win, cell->y, cell->x * 2, '0' + count);
            wattroff(win, COLOR_PAIR(count));
        } else {
            mvwaddch(win, cell->y, cell->x * 2, ' ');
        }
        wattroff(win, A_REVERSE);
    }
    wattroff(win, A_BOLD);
    wrefresh(win);
}

board_t new_board(int width, int height, int mines) {
    board_t board;
    board.width = width;
    board.height = height;
    board.mines = mines;
    board.current_cell = 0;
    board.covered = width * height;
    board.flags = 0;
    board.game_state = SETUP;
    board.draw_cursor = true;

    int i, j;
    cell_t *cells = calloc(width * height, sizeof(cell_t));
    for (i = 0; i < width; i++) {
        for (j = 0; j < height; j++) {
            int index = i * height + j;
            cells[index].x = i;
            cells[index].y = j;
            cells[index].covered = true;
            cells[index].flag = false;
            cells[index].mine = false;
            cells[index].mines_around = 0;
        }
    }
    board.cells = &cells[0];

    return board;
}

void set_mines(board_t *board) {
    int mines = 0;
    while (mines < board->mines) {
        int cell = rand() % (board->width * board->height);
        if (board->current_cell != cell && !board->cells[cell].mine) {
            board->cells[cell].mine = true;
            /* up */
            if (cell % board->height != 0) {
                board->cells[cell - 1].mines_around++;
            }
            /* down */
            if ((cell + 1) % board->height != 0) {
                board->cells[cell + 1].mines_around++;
            }
            /* left */
            if (cell - board->height >= 0) {
                board->cells[cell - board->height].mines_around++;
            }
            /* right */
            if (cell + board->height < board->width * board->height) {
                board->cells[cell + board->height].mines_around++;
            }
            /* up left */
            if (cell % board->height != 0 && cell - board->height >= 0) {
                board->cells[cell - 1 - board->height].mines_around++;
            }
            /* up right */
            if (cell % board->height != 0 &&
                cell + board->height < board->width * board->height) {
                board->cells[cell - 1 + board->height].mines_around++;
            }
            /* down left */
            if (((cell + 1) % board->height != 0) &&
                cell - board->height >= 0) {
                board->cells[cell + 1 - board->height].mines_around++;
            }
            /* down right */
            if (((cell + 1) % board->height != 0) &&
                cell + board->height < board->width * board->height) {
                board->cells[cell + 1 + board->height].mines_around++;
            }
            mines++;
        }
    }
}

void uncover_cell(board_t *board, int cell) {
    cell_t *cell_struct = &board->cells[cell];
    if (!cell_struct->flag && cell_struct->covered) {
        cell_struct->covered = false;
        board->covered--;
        if (cell_struct->mine) {
            board->game_state = BOOM;
            int i;
            for (i = 0; i < board->width * board->height; i++) {
                if (i != board->current_cell && board->cells[i].mine) {
                    board->cells[i].covered = false;
                }
            }
        } else if (cell_struct->mines_around == 0) {
            /* up */
            if (cell % board->height != 0) {
                uncover_cell(board, cell - 1);
            }
            /* down */
            if ((cell + 1) % board->height != 0) {
                uncover_cell(board, cell + 1);
            }
            /* left */
            if (cell - board->height >= 0) {
                uncover_cell(board, cell - board->height);
            }
            /* right */
            if (cell + board->height < board->width * board->height) {
                uncover_cell(board, cell + board->height);
            }
            /* up left */
            if (cell % board->height != 0 && cell - board->height >= 0) {
                uncover_cell(board, cell - 1 - board->height);
            }
            /* up right */
            if (cell % board->height != 0 &&
                cell + board->height < board->width * board->height) {
                uncover_cell(board, cell - 1 + board->height);
            }
            /* down left */
            if (((cell + 1) % board->height != 0) &&
                cell - board->height >= 0) {
                uncover_cell(board, cell + 1 - board->height);
            }
            /* down right */
            if (((cell + 1) % board->height != 0) &&
                cell + board->height < board->width * board->height) {
                uncover_cell(board, cell + 1 + board->height);
            }
        }
    }
}

void flood_uncover_cell(board_t *board, int cell) {
    cell_t *cell_struct = &board->cells[cell];
    if (!cell_struct->covered) {
        int flags_around = 0;
        /* up */
        if (cell % board->height != 0 && board->cells[cell - 1].flag) {
            flags_around++;
        }
        /* down */
        if ((cell + 1) % board->height != 0 && board->cells[cell + 1].flag) {
            flags_around++;
        }
        /* left */
        if (cell - board->height >= 0 &&
            board->cells[cell - board->height].flag) {
            flags_around++;
        }
        /* right */
        if (cell + board->height < board->width * board->height &&
            board->cells[cell + board->height].flag) {
            flags_around++;
        }
        /* up left */
        if (cell % board->height != 0 && cell - board->height >= 0 &&
            board->cells[cell - 1 - board->height].flag) {
            flags_around++;
        }
        /* up right */
        if (cell % board->height != 0 &&
            cell + board->height < board->width * board->height &&
            board->cells[cell - 1 + board->height].flag) {
            flags_around++;
        }
        /* down left */
        if (((cell + 1) % board->height != 0) && cell - board->height >= 0 &&
            board->cells[cell + 1 - board->height].flag) {
            flags_around++;
        }
        /* down right */
        if (((cell + 1) % board->height != 0) &&
            cell + board->height < board->width * board->height &&
            board->cells[cell + 1 + board->height].flag) {
            flags_around++;
        }
        if (cell_struct->mines_around == flags_around) {
            /* up */
            if (cell % board->height != 0) {
                uncover_cell(board, cell - 1);
            }
            /* down */
            if ((cell + 1) % board->height != 0) {
                uncover_cell(board, cell + 1);
            }
            /* left */
            if (cell - board->height >= 0) {
                uncover_cell(board, cell - board->height);
            }
            /* right */
            if (cell + board->height < board->width * board->height) {
                uncover_cell(board, cell + board->height);
            }
            /* up left */
            if (cell % board->height != 0 && cell - board->height >= 0) {
                uncover_cell(board, cell - 1 - board->height);
            }
            /* up right */
            if (cell % board->height != 0 &&
                cell + board->height < board->width * board->height) {
                uncover_cell(board, cell - 1 + board->height);
            }
            /* down left */
            if (((cell + 1) % board->height != 0) &&
                cell - board->height >= 0) {
                uncover_cell(board, cell + 1 - board->height);
            }
            /* down right */
            if (((cell + 1) % board->height != 0) &&
                cell + board->height < board->width * board->height) {
                uncover_cell(board, cell + 1 + board->height);
            }
        }
    }
}