#include "score.h"
#include <string.h>

static int fillRegion(const Board *b, int sx, int sy,
                      int visited[MAX_SIZE][MAX_SIZE],
                      int *adjBlack, int *adjWhite) {
  static const int dx[] = {1,-1,0,0};
  static const int dy[] = {0,0,1,-1};
  int stack[MAX_SIZE * MAX_SIZE][2];
  int top = 0, size = 0;
  *adjBlack = 0; *adjWhite = 0;

  stack[top][0] = sx; stack[top][1] = sy; top++;
  visited[sy][sx] = 1;

  while (top > 0) {
    top--;
    int x = stack[top][0], y = stack[top][1];
    size++;
    for (int d = 0; d < 4; d++) {
      int nx = x + dx[d], ny = y + dy[d];
      if (nx < 0 || nx >= b->size || ny < 0 || ny >= b->size) continue;
      if (b->cells[ny][nx] == BLACK) { *adjBlack = 1; continue; }
      if (b->cells[ny][nx] == WHITE) { *adjWhite = 1; continue; }
      if (!visited[ny][nx]) {
        visited[ny][nx] = 1;
        stack[top][0] = nx; stack[top][1] = ny; top++;
      }
    }
  }
  return size;
}

Score scoreCompute(const Board *b) {
  Score s = {0.0, 0.0, EMPTY};
  int visited[MAX_SIZE][MAX_SIZE];
  memset(visited, 0, sizeof(visited));

  for (int y = 0; y < b->size; y++)
    for (int x = 0; x < b->size; x++) {
      if (b->cells[y][x] == BLACK) s.black_score += 1.0;
      else if (b->cells[y][x] == WHITE) s.white_score += 1.0;
    }

  for (int y = 0; y < b->size; y++) {
    for (int x = 0; x < b->size; x++) {
      if (b->cells[y][x] != EMPTY || visited[y][x]) continue;
      int adjBlack, adjWhite;
      int region = fillRegion(b, x, y, visited, &adjBlack, &adjWhite);
      if (adjBlack && !adjWhite) s.black_score += region;
      else if (adjWhite && !adjBlack) s.white_score += region;
    }
  }

  s.white_score += KOMI;
  s.winner = s.black_score > s.white_score ? BLACK : WHITE;
  return s;
}
