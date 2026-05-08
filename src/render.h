#ifndef RENDER_H
#define RENDER_H

#include "game.h"

#define CP_DEFAULT 1
#define CP_CURSOR  2
#define CP_SIDEBAR 3
#define CP_HOSHI   4
#define CP_MSG     5

void renderInit(void);
void renderBoard(const GameState *gs);
void renderMessage(const char *msg);

#endif
