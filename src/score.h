#ifndef SCORE_H
#define SCORE_H

#include "board.h"

#define KOMI 7.5

typedef struct {
  double black_score;
  double white_score;
  Cell   winner;
} Score;

Score scoreCompute(const Board *b);

#endif
