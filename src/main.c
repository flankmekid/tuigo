#include <locale.h>
#include "platform.h"
#include "menu.h"
#include "game.h"
#include "render.h"

int main(void) {
  setlocale(LC_ALL, "");
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  renderInit();

  while (1) {
    MenuConfig cfg = menuRun();
    GameState  gs;
    gameInit(&gs, &cfg);
    gameRun(&gs);
  }

  endwin();
  return 0;
}
