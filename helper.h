#ifndef HELPER_H
#define HELPER_H

#include <stdlib.h>
#include <stdio.h>

typedef enum {
    LEFT=0, UP=1, RIGHT=2, DOWN=3
} Direction;

#define OPPOSITE_DIR(dir) ((dir + 2) % 4)

typedef struct {
    int x;
    int y;
} Vec2;

typedef Vec2 Position;

#define RANDOM(min, max) (rand() % (max-min) + min)

enum {
    FLAG_FIXED_SIZE     = 1,
    FLAG_AGENT_0        = 2,
    FLAG_AGENT_1        = 4,
    FLAG_LOG            = 8,
    FLAG_SEED           = 16,
};

typedef struct {
    int flags;
    int width;
    int height;
    int seed;
    int agent0;
    int agent1;
} Settings;

extern Settings settings;

Vec2 vec2Init(int x, int y);
Vec2 vec2Add(Vec2 a, Vec2 b);
Vec2 vec2Sub(Vec2 a, Vec2 b);
Vec2 vec2Mul(Vec2 a, int s);
unsigned int vec2Dist(Vec2 a, Vec2 b);
Vec2 dirToVec2(Direction dir);
void vec2LimitRange(Vec2 *pos, int ll, int lr, int lt, int lb);
Vec2 vec2Random(int ll, int lr, int lt, int lb);
int vec2Equal(Vec2 a, Vec2 b);

void parseArgs(Settings *settings, int argc, const char *argv[]);
void logMsg(const char *fmt, ...);

#endif
