#ifndef GAME_H
#define GAME_H

#include "board.h"
#include "menu.h"
#include "input.h"

typedef struct {
  Board       board;
  MoveHistory history;
  GameMode    mode;
  int         difficulty;
  Cell        humanColor;
  int         cursorX, cursorY;
} GameState;

void gameInit(GameState *gs, const MenuConfig *cfg);
void gameRun(GameState *gs);

#endif
