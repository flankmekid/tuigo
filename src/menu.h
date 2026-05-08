#ifndef MENU_H
#define MENU_H

#include "board.h"

typedef enum { MODE_HVH = 0, MODE_HVAI = 1 } GameMode;

typedef struct {
  GameMode mode;
  int      size;
  int      difficulty;
  Cell     humanColor;
} MenuConfig;

MenuConfig menuRun(void);

#endif
