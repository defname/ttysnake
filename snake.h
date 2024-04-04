#ifndef SNAKE_H
#define SNAKE_H

#include "common.h"
#include "helper.h"

typedef struct {
    int length;
    Direction dir;
    Position body[SNAKE_MAX_LENGTH];
} Snake;

#define FIX_WINNER(snake1, snake2, winner) (snake1.dir == OPPOSITE_DIR(snake2.dir) ? 0 : winner)

void snakeInit(Snake *snake, int length, int startX, int startY, Direction dir);
void snakeMove(Snake *snake, int scrWidth, int scrHeight);
void snakeChangeDirection(Snake *snake, Direction dir);
void snakeDraw(Snake *snake);
int snakeCheckCollision(Snake *snake, Snake *enemy);
#endif
