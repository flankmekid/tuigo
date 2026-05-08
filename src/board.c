#include "board.h"
#include <string.h>

Cell board_opponent(Cell c) { return c == BLACK ? WHITE : BLACK; }

void board_init(Board *b, int size) {
  memset(b, 0, sizeof(Board));
  b->size = size;
  b->turn = BLACK;
  b->ko_x = -1;
  b->ko_y = -1;
}

static void flood_liberties(const Board *b, int x, int y, Cell color,
                            int visited[MAX_SIZE][MAX_SIZE], int *libs) {
  static const int dx[] = {1, -1, 0, 0};
  static const int dy[] = {0, 0, 1, -1};
  int stack[MAX_SIZE * MAX_SIZE][2];
  int top = 0;

  if (x < 0 || x >= b->size || y < 0 || y >= b->size) return;
  if (visited[y][x] || b->cells[y][x] != color) return;

  stack[top][0] = x; stack[top][1] = y; top++;
  visited[y][x] = 1;

  while (top > 0) {
    top--;
    int cx = stack[top][0], cy = stack[top][1];
    for (int d = 0; d < 4; d++) {
      int nx = cx + dx[d], ny = cy + dy[d];
      if (nx < 0 || nx >= b->size || ny < 0 || ny >= b->size) continue;
      if (visited[ny][nx]) continue;
      visited[ny][nx] = 1;
      if (b->cells[ny][nx] == EMPTY) { (*libs)++; continue; }
      if (b->cells[ny][nx] == color) { stack[top][0] = nx; stack[top][1] = ny; top++; }
    }
  }
}

static void flood_remove(Board *b, int x, int y, Cell color, int *count) {
  static const int dx[] = {1, -1, 0, 0};
  static const int dy[] = {0, 0, 1, -1};
  int stack[MAX_SIZE * MAX_SIZE][2];
  int top = 0;

  if (x < 0 || x >= b->size || y < 0 || y >= b->size) return;
  if (b->cells[y][x] != color) return;

  b->cells[y][x] = EMPTY;
  (*count)++;
  stack[top][0] = x; stack[top][1] = y; top++;

  while (top > 0) {
    top--;
    int cx = stack[top][0], cy = stack[top][1];
    for (int d = 0; d < 4; d++) {
      int nx = cx + dx[d], ny = cy + dy[d];
      if (nx < 0 || nx >= b->size || ny < 0 || ny >= b->size) continue;
      if (b->cells[ny][nx] != color) continue;
      b->cells[ny][nx] = EMPTY;
      (*count)++;
      stack[top][0] = nx; stack[top][1] = ny; top++;
    }
  }
}

int board_count_liberties(const Board *b, int x, int y) {
  if (b->cells[y][x] == EMPTY)
    return 0;

  int visited[MAX_SIZE][MAX_SIZE] = {{0}};
  int libs = 0;
  flood_liberties(b, x, y, b->cells[y][x], visited, &libs);
  return libs;
}

void board_place_unchecked(Board *b, int x, int y) {
  static const int dx[] = {1, -1, 0, 0};
  static const int dy[] = {0, 0, 1, -1};

  b->cells[y][x] = b->turn;
  b->pass_count = 0;

  Cell enemy = board_opponent(b->turn);
  int total = 0;
  int groups = 0;
  int last_x = -1, last_y = -1;

  for (int d = 0; d < 4; d++) {
    int nx = x + dx[d];
    int ny = y + dy[d];

    if (nx < 0 || nx >= b->size || ny < 0 || ny >= b->size)
      continue;
    if (b->cells[ny][nx] != enemy)
      continue;
    if (board_count_liberties(b, nx, ny) > 0)
      continue;

    int cnt = 0;
    last_x = nx;
    last_y = ny;
    flood_remove(b, nx, ny, enemy, &cnt);
    total += cnt;
    groups++;
    if (cnt != 1) {
      last_x = -1;
      last_y = -1;
    }
  }

  if (total == 1 && groups == 1 && last_x != -1) {
    b->ko_x = last_x;
    b->ko_y = last_y;
  } else {
    b->ko_x = -1;
    b->ko_y = -1;
  }

  if (b->turn == BLACK)
    b->white_captured += total;
  else
    b->black_captured += total;

  b->turn = board_opponent(b->turn);
}

void board_pass(Board *b) {
  b->pass_count++;
  b->turn  = board_opponent(b->turn);
  b->ko_x  = -1;
  b->ko_y  = -1;
}

void historyInit(MoveHistory *h) { h->top = 0; }

void historyPush(MoveHistory *h, const Board *b) {
  h->boards[h->top % HISTORY_CAP] = *b;
  h->top++;
}

int historyUndo(MoveHistory *h, Board *b) {
  if (h->top == 0) return 0;
  h->top--;
  *b = h->boards[h->top % HISTORY_CAP];
  return 1;
}
