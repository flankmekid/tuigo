#include "rules.h"

int rulesIsLegal(const Board *b, int x, int y) {
  if (x < 0 || x >= b->size || y < 0 || y >= b->size) return 0;
  if (b->cells[y][x] != EMPTY) return 0;
  if (x == b->ko_x && y == b->ko_y) return 0;

  /* Fast path: an empty orthogonal neighbor means the stone will have at
     least one liberty — legal without touching the board copy. */
  static const int dx[] = {1, -1, 0, 0};
  static const int dy[] = {0,  0, 1, -1};
  for (int d = 0; d < 4; d++) {
    int nx = x + dx[d], ny = y + dy[d];
    if (nx >= 0 && nx < b->size && ny >= 0 && ny < b->size
        && b->cells[ny][nx] == EMPTY)
      return 1;
  }

  /* Slow path: cell is surrounded — need to check captures / suicide. */
  Board tmp = *b;
  board_place_unchecked(&tmp, x, y);
  return board_count_liberties(&tmp, x, y) > 0;
}
