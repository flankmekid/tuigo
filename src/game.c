#include "game.h"
#include "rules.h"
#include "score.h"
#include "render.h"
#include "ai.h"
#include "platform.h"
#include <stdio.h>

void gameInit(GameState *gs, const MenuConfig *cfg) {
  board_init(&gs->board, cfg->size);
  historyInit(&gs->history);
  gs->mode       = cfg->mode;
  gs->difficulty = cfg->difficulty;
  gs->humanColor = cfg->humanColor;
  gs->cursorX    = cfg->size / 2;
  gs->cursorY    = cfg->size / 2;
}

static void showScore(const GameState *gs) {
  Score s = scoreCompute(&gs->board);
  char buf[128];
  snprintf(buf, sizeof(buf),
           "%s wins — black: %.1f  white: %.1f  (any key)",
           s.winner == BLACK ? "black" : "white",
           s.black_score, s.white_score);
  renderBoard(gs);
  renderMessage(buf);
  getch();
}

void gameRun(GameState *gs) {
  int size = gs->board.size;

  while (1) {
    renderBoard(gs);

    if (gs->mode == MODE_HVAI && gs->board.turn != gs->humanColor) {
      int ax, ay;
      aiBestMove(&gs->board, gs->difficulty, &ax, &ay);
      historyPush(&gs->history, &gs->board);
      if (ax == -1) board_pass(&gs->board);
      else          board_place_unchecked(&gs->board, ax, ay);
      if (gs->board.pass_count >= 2) { showScore(gs); return; }
      continue;
    }

    Action a = inputGetAction();
    switch (a) {
      case ACTION_UP:    if (gs->cursorY < size - 1) gs->cursorY++; break;
      case ACTION_DOWN:  if (gs->cursorY > 0)        gs->cursorY--; break;
      case ACTION_LEFT:  if (gs->cursorX > 0)        gs->cursorX--; break;
      case ACTION_RIGHT: if (gs->cursorX < size - 1) gs->cursorX++; break;
      case ACTION_PLACE:
        if (rulesIsLegal(&gs->board, gs->cursorX, gs->cursorY)) {
          historyPush(&gs->history, &gs->board);
          board_place_unchecked(&gs->board, gs->cursorX, gs->cursorY);
          if (gs->board.pass_count >= 2) { showScore(gs); return; }
        }
        break;
      case ACTION_PASS:
        historyPush(&gs->history, &gs->board);
        board_pass(&gs->board);
        if (gs->board.pass_count >= 2) { showScore(gs); return; }
        break;
      case ACTION_UNDO:
        historyUndo(&gs->history, &gs->board);
        if (gs->mode == MODE_HVAI && gs->history.top > 0)
          historyUndo(&gs->history, &gs->board);
        break;
      case ACTION_QUIT: return;
      default: break;
    }
  }
}
