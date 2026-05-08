#include "menu.h"
#include "input.h"
#include "platform.h"
#include <stdlib.h>

static const char *MODE_LABELS[] = {"Human vs Human", "Human vs AI"};
static const char *SIZE_LABELS[] = {"9x9", "13x13", "19x19"};
static const char *DIFF_LABELS[] = {"Easy", "Normal", "Hard", "Very Hard"};
static const char *COLOR_LABELS[] = {"Play as Black", "Play as White"};
static const int SIZE_VALUES[] = {9, 13, 19};
static const int DIFF_BUDGETS[] = {300, 1000, 3000, 8000}; /* ms */

static void drawMenu(int mode, int size, int diff, int color, int row) {
  int rows, cols;
  getmaxyx(stdscr, rows, cols);
  int c = cols / 2 - 20;
  int r = rows / 2 - 4;
  clear();

  attron(A_BOLD);
  mvprintw(r - 2, c, "tuigo - weiqi for the terminal");
  attroff(A_BOLD);

  mvprintw(r, c - 12, "Mode:");
  for (int i = 0; i < 2; i++) {
    if (row == 0 && i == mode)
      attron(A_REVERSE);
    mvprintw(r, c + i * 20, "[ %s ]", MODE_LABELS[i]);
    if (row == 0 && i == mode)
      attroff(A_REVERSE);
  }

  mvprintw(r + 2, c - 12, "Board:");
  for (int i = 0; i < 3; i++) {
    if (row == 1 && i == size)
      attron(A_REVERSE);
    mvprintw(r + 2, c + i * 10, "[ %s ]", SIZE_LABELS[i]);
    if (row == 1 && i == size)
      attroff(A_REVERSE);
  }

  if (mode == MODE_HVAI) {
    mvprintw(r + 4, c - 12, "Difficulty:");
    for (int i = 0; i < 4; i++) {
      if (row == 2 && i == diff)
        attron(A_REVERSE);
      mvprintw(r + 4, c + i * 12, "[ %s ]", DIFF_LABELS[i]);
      if (row == 2 && i == diff)
        attroff(A_REVERSE);
    }

    mvprintw(r + 6, c - 12, "Color:");
    for (int i = 0; i < 2; i++) {
      if (row == 3 && i == color)
        attron(A_REVERSE);
      mvprintw(r + 6, c + i * 18, "[ %s ]", COLOR_LABELS[i]);
      if (row == 3 && i == color)
        attroff(A_REVERSE);
    }
  }

  mvprintw(r + 9, c, "[arrows/hjkl] navigate   [Enter] start   [q] quit");
  refresh();
}

MenuConfig menuRun(void) {
  int selMode = 0, selSize = 0, selDiff = 1, selColor = 0, activeRow = 0;

  while (1) {
    int isAi = (selMode == MODE_HVAI);
    int maxRow = isAi ? 3 : 1;
    drawMenu(selMode, selSize, selDiff, selColor, activeRow);

    Action a = inputGetAction();
    if (a == ACTION_QUIT) {
      endwin();
      exit(0);
    }
    if (a == ACTION_PLACE)
      break;

    if (a == ACTION_DOWN)
      activeRow = (activeRow < maxRow) ? activeRow + 1 : activeRow;
    if (a == ACTION_UP)
      activeRow = (activeRow > 0) ? activeRow - 1 : 0;

    if (a == ACTION_LEFT || a == ACTION_RIGHT) {
      int d = (a == ACTION_RIGHT) ? 1 : -1;
      if (activeRow == 0)
        selMode = (selMode + d + 2) % 2;
      if (activeRow == 1)
        selSize = (selSize + d + 3) % 3;
      if (activeRow == 2)
        selDiff = (selDiff + d + 4) % 4;
      if (activeRow == 3)
        selColor = (selColor + d + 2) % 2;
    }
  }

  MenuConfig cfg;
  cfg.mode = selMode;
  cfg.size = SIZE_VALUES[selSize];
  cfg.difficulty = DIFF_BUDGETS[selDiff];
  cfg.humanColor = (selColor == 0) ? BLACK : WHITE;
  return cfg;
}
