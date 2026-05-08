#include "input.h"
#include "platform.h"

Action inputGetAction(void) {
  int ch = getch();
  switch (ch) {
    case KEY_UP:    case 'k': return ACTION_UP;
    case KEY_DOWN:  case 'j': return ACTION_DOWN;
    case KEY_LEFT:  case 'h': return ACTION_LEFT;
    case KEY_RIGHT: case 'l': return ACTION_RIGHT;
    case '\n': case '\r': case KEY_ENTER: return ACTION_PLACE;
    case 'p': case 'P': return ACTION_PASS;
    case 'u': case 'U': return ACTION_UNDO;
    case 'q': case 'Q': return ACTION_QUIT;
    case KEY_RESIZE:
      resizeterm(LINES, COLS);
      clearok(stdscr, TRUE);
      return ACTION_NONE;
    default: return ACTION_NONE;
  }
}
