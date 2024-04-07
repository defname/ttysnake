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

void snakeInit(Snake *snake, int id, int length, int startX, int startY, Direction dir) {
    Position position = {startX, startY};
    snake->id = id;
    snake->length = length;
    snake->dir = dir;
    snake->alive = 1;
    snake->body[0] = position;
    Vec2 d = vec2Mul(dirToVec2(dir), -1);

    for (int i=1; i<length; i++) {
        snake->body[i] = vec2Add(snake->body[i-1], d);
    }
}

void snakeMove(Snake *snake, int scrWidth, int scrHeight) {
    Position head = vec2Add(snake->body[0], dirToVec2(snake->dir));
    //vec2LimitRange(&head, 1, scrWidth-1, 1, scrHeight-1);
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
    int symbol = SNAKE_SYMBOL;
    mvaddch(snake->body[0].y, snake->body[0].x, symbol);
    Position last, current, next;

    for (int i=1; i<snake->length-1; i++) {
        last = snake->body[i-1];
        current = snake->body[i];
        next = snake->body[i+1];

        Vec2 a = vec2Sub(last, current);
        Vec2 b = vec2Sub(current, next);
        symbol = 'x';
        if (a.x == 0 && b.x == 0) {
            symbol = ACS_VLINE;
        }
        else if (a.y == 0 && b.y == 0) {
            symbol = ACS_HLINE;
        }
        else if (a.x == 1 && b.y == 1) {
            symbol = ACS_LLCORNER;
        }
        else if (a.x == -1 && b.y == 1) {
            symbol = ACS_LRCORNER;
        }
        else if (a.x == 1 && b.y == -1) {
            symbol = ACS_ULCORNER;
        }
        else if (a.x == -1 && b.y == -1) {
            symbol = ACS_URCORNER;
        }
        else if (a.y == 1 && b.x == 1) {
            symbol = ACS_URCORNER;
        }
        else if (a.y == -1 && b.x == 1) {
            symbol = ACS_LRCORNER;
        }
        else if (a.y == 1 && b.x == -1) {
            symbol = ACS_ULCORNER;
        }
        else if (a.y == -1 && b.x == -1) {
            symbol = ACS_LLCORNER;
        }
        mvaddch(snake->body[i].y, snake->body[i].x, symbol);
    }
    Vec2 d = vec2Sub(current, next);
    if (d.x == 0)   symbol = ACS_VLINE;
    else            symbol = ACS_HLINE;
}

int snakeCheckCollision(Snake *snake, Snake *enemy, int width, int height) {
    Position head = snake->body[0];
    /* check for wall collision */
    if (head.x == 0 || head.x == width-1 || head.y == 0 || head.y == height-1) {
        return 1;
    }

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
