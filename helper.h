#ifndef HELPER_H
#define HELPER_H

#include <stdlib.h>

typedef enum {
    LEFT, UP, RIGHT, DOWN
} Direction;

#define OPPOSITE_DIR(dir) ((dir + 2) % 4)

typedef struct {
    int x;
    int y;
} Vec2;

typedef Vec2 Position;

#define RANDOM(min, max) (rand() % (max-min) + min)

Vec2 vec2Init(int x, int y);
Vec2 vec2Add(Vec2 a, Vec2 b);
Vec2 vec2Mul(Vec2 a, int s);
Vec2 dirToVec2(Direction dir);
void vec2LimitRange(Vec2 *pos, int ll, int lr, int lt, int lb);
Vec2 vec2Random(int ll, int lr, int lt, int lb);
int vec2Equal(Vec2 a, Vec2 b);

#endif
