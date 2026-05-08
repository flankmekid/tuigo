#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

#define MAX_SIZE    19
#define HISTORY_CAP 512

typedef enum { EMPTY = 0, BLACK = 1, WHITE = 2 } Cell;

typedef struct {
  uint8_t cells[MAX_SIZE][MAX_SIZE];
  int size;
  Cell turn;
  int ko_x, ko_y;
  int pass_count;
  int black_captured, white_captured;
} Board;

typedef struct {
  Board boards[HISTORY_CAP];
  int top;
} MoveHistory;

Cell board_opponent(Cell c);
void board_init(Board *b, int size);
void board_place_unchecked(Board *b, int x, int y);
void board_pass(Board *b);
int  board_count_liberties(const Board *b, int x, int y);

void historyInit(MoveHistory *h);
void historyPush(MoveHistory *h, const Board *b);
int  historyUndo(MoveHistory *h, Board *b);

#endif
