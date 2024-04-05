#include <curses.h>
#include "snake.h"
/**
 * Add a and b respecting limits
 */
static int addRespectingLimits(int a, int b, int lowerLimit, int upperLimit) {
    int result = a + b;
    if (result < lowerLimit) return upperLimit - (lowerLimit - result);
    if (result >= upperLimit) return lowerLimit - (result - upperLimit);
    return result;
}

void snakeInit(Snake *snake, int length, int startX, int startY, Direction dir) {
    Position position = {startX, startY};
    snake->length = length;
    snake->dir = dir;
    snake->body[0] = position;
    Vec2 d = vec2Mul(dirToVec2(dir), -1);

    for (int i=1; i<length; i++) {
        snake->body[i] = vec2Add(snake->body[i-1], d);
    }
}

void snakeMove(Snake *snake, int scrWidth, int scrHeight) {
    Position head = vec2Add(snake->body[0], dirToVec2(snake->dir));
    vec2LimitRange(&head, 1, scrWidth-1, 1, scrHeight-1);
    for (int i=snake->length-1; i>0; i--) {
        snake->body[i] = snake->body[i-1];
    }
    snake->body[0] = head;
}

void snakeChangeDirection(Snake *snake, Direction dir) {
    if (snake->dir != OPPOSITE_DIR(dir))
        snake->dir = dir;
}

void snakeDraw(Snake *snake) {
    for (int i=0; i<snake->length; i++) {
        mvaddch(snake->body[i].y, snake->body[i].x, 'O');
    }
}

int snakeCheckCollision(Snake *snake, Snake *enemy) {
    Position head = snake->body[0];
    /* check for self collision */
    for (int i=1; i<snake->length; i++) {
        if (vec2Equal(head, snake->body[i])) return 1;
    }
    /* check enemy */
    for (int i=0; i<enemy->length; i++) {
        if (vec2Equal(head, enemy->body[i])) return 1;
    }

    return 0;
}
