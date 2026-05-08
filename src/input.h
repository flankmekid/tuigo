#ifndef INPUT_H
#define INPUT_H

typedef enum {
  ACTION_UP, ACTION_DOWN, ACTION_LEFT, ACTION_RIGHT,
  ACTION_PLACE, ACTION_PASS, ACTION_UNDO, ACTION_QUIT,
  ACTION_NONE
} Action;

Action inputGetAction(void);

#endif
