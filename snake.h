#ifndef SNAKE_H
#define SNAKE_H

#include "common.h"
#include "helper.h"

typedef struct {
    int id;
    int length;
    int alive;
    Direction dir;
    Position body[SNAKE_MAX_LENGTH];
} Snake;

void snakeInit(Snake *snake, int id, int length, int startX, int startY, Direction dir);
void snakeMove(Snake *snake, int scrWidth, int scrHeight);
void snakeChangeDirection(Snake *snake, Direction dir);
void snakeDraw(Snake *snake);
void snakeDrawHead(Snake *snake);
int snakeCheckCollision(Snake *snake, Snake *enemy, int width, int height);
#endif
