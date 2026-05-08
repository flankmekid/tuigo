#include "render.h"
#include "platform.h"
#include <string.h>
#include <wchar.h>

static const char *COL_LABELS = "ABCDEFGHJKLMNOPQRST";

static const int HOSHI_9[][2]  = {{2,2},{6,2},{2,6},{6,6},{4,4}};
static const int HOSHI_13[][2] = {{3,3},{9,3},{3,9},{9,9},{6,6}};
static const int HOSHI_19[][2] = {{3,3},{9,3},{15,3},{3,9},{9,9},{15,9},{3,15},{9,15},{15,15}};

static int isHoshi(int size, int x, int y) {
  const int (*h)[2];
  int cnt;
  if      (size == 9)  { h = HOSHI_9;  cnt = 5; }
  else if (size == 13) { h = HOSHI_13; cnt = 5; }
  else                 { h = HOSHI_19; cnt = 9; }
  for (int i = 0; i < cnt; i++)
    if (h[i][0] == x && h[i][1] == y) return 1;
  return 0;
}

static const wchar_t *gridChar(int top, int bot, int left, int right) {
  if (top  && left)  return L"┌"; /* ┌ */
  if (top  && right) return L"┐"; /* ┐ */
  if (bot  && left)  return L"└"; /* └ */
  if (bot  && right) return L"┘"; /* ┘ */
  if (top)           return L"┬"; /* ┬ */
  if (bot)           return L"┴"; /* ┴ */
  if (left)          return L"├"; /* ├ */
  if (right)         return L"┤"; /* ┤ */
  return             L"┼";        /* ┼ */
}

#define S_BLACK L"●"   /* ● */
#define S_WHITE L"○"   /* ○ */
#define S_HOSHI L"╬"   /* ╬ */
#define S_HORIZ L"─"   /* ─ */
#define S_VERT  L"│"   /* │ */

void renderInit(void) {
  start_color();
  use_default_colors();
  init_pair(CP_DEFAULT, -1,         -1);
  init_pair(CP_CURSOR,  COLOR_BLACK, COLOR_YELLOW);
  init_pair(CP_SIDEBAR, COLOR_CYAN,  -1);
  init_pair(CP_HOSHI,   COLOR_GREEN, -1);
  init_pair(CP_MSG,     COLOR_BLACK, COLOR_WHITE);
}

void renderBoard(const GameState *gs) {
  int termRows, termCols;
  getmaxyx(stdscr, termRows, termCols);

  const Board *b = &gs->board;
  int size       = b->size;
  int origRow    = 1;
  int origCol    = 4;
  int sidebar    = 22;

  /* scale: how many chars per cell horizontally, rows per cell vertically */
  int availW = termCols - origCol - 4 - sidebar;
  int availH = termRows - 4;
  int sh = availW / (size * 2);
  int sv = availH / size;
  int scale = sh < sv ? sh : sv;
  if (scale < 1) scale = 1;
  if (scale > 5) scale = 5;

  int cellW = scale * 2;  /* chars between intersections (horiz step) */
  int cellH = scale;      /* rows between intersections (vert step)   */

  clear();

  /* column labels (top) */
  attron(COLOR_PAIR(CP_SIDEBAR));
  for (int x = 0; x < size; x++)
    mvprintw(origRow, origCol + x * cellW, "%c", COL_LABELS[x]);
  attroff(COLOR_PAIR(CP_SIDEBAR));

  for (int y = size - 1; y >= 0; y--) {
    int sr = origRow + 1 + (size - 1 - y) * cellH;

    /* row label (left) */
    attron(COLOR_PAIR(CP_SIDEBAR));
    mvprintw(sr, 0, "%2d ", y + 1);
    attroff(COLOR_PAIR(CP_SIDEBAR));

    /* intersections + horizontal connectors */
    for (int x = 0; x < size; x++) {
      int sc     = origCol + x * cellW;
      int cursor = (x == gs->cursorX && y == gs->cursorY);
      Cell cell  = b->cells[y][x];

      move(sr, sc);
      if (cursor) attron(COLOR_PAIR(CP_CURSOR));

      if      (cell == BLACK) addwstr(S_BLACK);
      else if (cell == WHITE) addwstr(S_WHITE);
      else if (isHoshi(size, x, y)) {
        if (!cursor) attron(COLOR_PAIR(CP_HOSHI));
        addwstr(S_HOSHI);
        if (!cursor) attroff(COLOR_PAIR(CP_HOSHI));
      } else {
        addwstr(gridChar(y == size-1, y == 0, x == 0, x == size-1));
      }

      if (cursor) attroff(COLOR_PAIR(CP_CURSOR));

      /* horizontal connectors to next column */
      if (x < size - 1)
        for (int c = 0; c < cellW - 1; c++) addwstr(S_HORIZ);
    }

    /* row label (right) */
    attron(COLOR_PAIR(CP_SIDEBAR));
    printw(" %d", y + 1);
    attroff(COLOR_PAIR(CP_SIDEBAR));

    /* vertical connector rows between board rows */
    if (y > 0) {
      for (int vc = 1; vc < cellH; vc++) {
        int vcr = sr + vc;
        for (int x = 0; x < size; x++) {
          int sc = origCol + x * cellW;
          move(vcr, sc);
          addwstr(S_VERT);
        }
      }
    }
  }

  /* column labels (bottom) */
  int bottomRow = origRow + 1 + (size - 1) * cellH + 1;
  attron(COLOR_PAIR(CP_SIDEBAR));
  for (int x = 0; x < size; x++)
    mvprintw(bottomRow, origCol + x * cellW, "%c", COL_LABELS[x]);
  attroff(COLOR_PAIR(CP_SIDEBAR));

  /* sidebar */
  int sb = origCol + (size - 1) * cellW + 5;
  attron(COLOR_PAIR(CP_SIDEBAR));
  mvprintw(origRow,     sb, "tuigo  %dx%d", size, size);
  mvprintw(origRow + 2, sb, "turn: %s (", b->turn == BLACK ? "black" : "white");
  addwstr(b->turn == BLACK ? S_BLACK : S_WHITE);
  addch(')');
  mvprintw(origRow + 4, sb, "captured:");
  mvprintw(origRow + 5, sb, "  "); addwstr(S_BLACK); printw(" %d", b->black_captured);
  mvprintw(origRow + 6, sb, "  "); addwstr(S_WHITE); printw(" %d", b->white_captured);
  mvprintw(origRow + 8,  sb, "[hjkl] move");
  mvprintw(origRow + 9,  sb, "[enter] place");
  mvprintw(origRow + 10, sb, "[p] pass");
  mvprintw(origRow + 11, sb, "[u] undo");
  mvprintw(origRow + 12, sb, "[q] quit");
  attroff(COLOR_PAIR(CP_SIDEBAR));

  refresh();
}

void renderMessage(const char *msg) {
  int rows, cols;
  getmaxyx(stdscr, rows, cols);
  int len = (int)strlen(msg) + 4;
  attron(COLOR_PAIR(CP_MSG));
  mvprintw(rows / 2, (cols - len) / 2, "  %s  ", msg);
  attroff(COLOR_PAIR(CP_MSG));
  refresh();
}
