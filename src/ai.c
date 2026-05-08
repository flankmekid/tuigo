#include "ai.h"
#include "rules.h"
#include "score.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define UCB_C 1.414
#define MAX_MOVES (MAX_SIZE * MAX_SIZE + 1)

typedef struct Node {
  Board board;
  int mx, my;
  int visits;
  double wins;
  struct Node *parent;
  struct Node **children;
  int childCount, childCap;
  int fullyExpanded;
} Node;

static Node *nodeAlloc(const Board *b, int mx, int my, Node *parent) {
  Node *n = calloc(1, sizeof(Node));
  if (!n)
    return NULL;
  n->board = *b;
  n->mx = mx;
  n->my = my;
  n->parent = parent;
  return n;
}

static void nodeFree(Node *n) {
  if (!n)
    return;
  for (int i = 0; i < n->childCount; i++)
    nodeFree(n->children[i]);
  free(n->children);
  free(n);
}

static int getLegalMoves(const Board *b, int moves[MAX_MOVES][2]) {
  int cnt = 0;
  for (int y = 0; y < b->size; y++)
    for (int x = 0; x < b->size; x++)
      if (rulesIsLegal(b, x, y)) {
        moves[cnt][0] = x;
        moves[cnt][1] = y;
        cnt++;
      }
  moves[cnt][0] = -1;
  moves[cnt][1] = -1;
  cnt++;
  return cnt;
}

static double ucb1(const Node *child, int parentVisits) {
  if (child->visits == 0)
    return 1e18;
  return (child->wins / child->visits) +
         UCB_C * sqrt(log((double)parentVisits) / child->visits);
}

static Node *bestChildUcb(Node *n) {
  Node *best = NULL;
  double bestScore = -1e18;
  for (int i = 0; i < n->childCount; i++) {
    double s = ucb1(n->children[i], n->visits);
    if (s > bestScore) {
      bestScore = s;
      best = n->children[i];
    }
  }
  return best;
}

static Node *expand(Node *n) {
  int moves[MAX_MOVES][2];
  int total = getLegalMoves(&n->board, moves);

  for (int i = 0; i < total; i++) {
    int mx = moves[i][0], my = moves[i][1];
    int tried = 0;
    for (int j = 0; j < n->childCount; j++)
      if (n->children[j]->mx == mx && n->children[j]->my == my) {
        tried = 1;
        break;
      }
    if (tried)
      continue;

    Board next = n->board;
    if (mx == -1)
      board_pass(&next);
    else
      board_place_unchecked(&next, mx, my);

    Node *child = nodeAlloc(&next, mx, my, n);
    if (!child) {
      n->fullyExpanded = 1;
      return n;
    }
    if (n->childCount == n->childCap) {
      int newCap = n->childCap ? n->childCap * 2 : 8;
      Node **newChildren = realloc(n->children, newCap * sizeof(Node *));
      if (!newChildren) {
        nodeFree(child);
        n->fullyExpanded = 1;
        return n;
      }
      n->children = newChildren;
      n->childCap = newCap;
    }
    n->children[n->childCount++] = child;
    if (n->childCount == total)
      n->fullyExpanded = 1;
    return child;
  }
  n->fullyExpanded = 1;
  return n;
}

static Cell simulate(Board b) {
  int consec = 0;
  int maxMoves = b.size * b.size * 3;

  while (consec < 2 && maxMoves-- > 0) {
    int empty[MAX_SIZE * MAX_SIZE][2];
    int n = 0;
    for (int y = 0; y < b.size; y++)
      for (int x = 0; x < b.size; x++)
        if (b.cells[y][x] == EMPTY) {
          empty[n][0] = x;
          empty[n][1] = y;
          n++;
        }

    int placed = 0;
    for (int i = 0; i < n && !placed; i++) {
      int j = i + rand() % (n - i);
      int tx = empty[i][0], ty = empty[i][1];
      empty[i][0] = empty[j][0];
      empty[i][1] = empty[j][1];
      empty[j][0] = tx;
      empty[j][1] = ty;
      if (rulesIsLegal(&b, empty[i][0], empty[i][1])) {
        board_place_unchecked(&b, empty[i][0], empty[i][1]);
        consec = 0;
        placed = 1;
      }
    }
    if (!placed) {
      board_pass(&b);
      consec++;
    }
  }
  return scoreCompute(&b).winner;
}

static void backprop(Node *n, Cell winner) {
  while (n) {
    n->visits++;
    Cell mover = board_opponent(n->board.turn);
    if (mover == winner)
      n->wins += 1.0;
    n = n->parent;
  }
}

static long msNow(void) { return (long)(clock()) * 1000L / CLOCKS_PER_SEC; }

void aiBestMove(const Board *b, int budget_ms, int *outX, int *outY) {
  static int seeded = 0;
  if (!seeded) {
    srand((unsigned)time(NULL));
    seeded = 1;
  }

  Node *root = nodeAlloc(b, -2, -2, NULL);
  if (!root) {
    *outX = -1;
    *outY = -1;
    return;
  }

  long deadline = msNow() + budget_ms;
  for (int i = 0;; i++) {
    if ((i & 63) == 0 && msNow() >= deadline)
      break;
    Node *node = root;
    while (node->fullyExpanded && node->childCount > 0)
      node = bestChildUcb(node);
    if (!node->fullyExpanded)
      node = expand(node);
    Cell winner = simulate(node->board);
    backprop(node, winner);
  }

  Node *best = NULL;
  int bestVisits = -1;
  for (int i = 0; i < root->childCount; i++)
    if (root->children[i]->visits > bestVisits) {
      bestVisits = root->children[i]->visits;
      best = root->children[i];
    }

  *outX = best ? best->mx : -1;
  *outY = best ? best->my : -1;
  nodeFree(root);
}
