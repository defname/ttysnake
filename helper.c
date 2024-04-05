#include "helper.h"

Vec2 vec2Init(int x, int y) {
    Vec2 a = {x, y};
    return a;
}

Vec2 vec2Add(Vec2 a, Vec2 b) {
    Vec2 result = {a.x + b.x, a.y + b.y};
    return result;
}

Vec2 vec2Mul(Vec2 a, int s) {
    Vec2 result = {a.x * s, a.y * s};
    return result;
}

unsigned int vec2Dist(Vec2 a, Vec2 b) {
    return abs(a.x-b.x) + abs(a.y-b.y);
}

Vec2 dirToVec2(Direction dir) {
    Vec2 v;
    switch (dir) {
        case LEFT:
            v.x = -1;
            v.y = 0;
            break;
        case RIGHT:
            v.x = 1;
            v.y = 0;
            break;
        case UP:
            v.x = 0;
            v.y = -1;
            break;
        case DOWN:
            v.x = 0;
            v.y = 1;
            break;
    }
    return v;
}

void vec2LimitRange(Vec2 *pos, int ll, int lr, int lt, int lb) {
    if (pos->x < ll) pos->x = lr - (ll - pos->x);
    else if (lr <= pos->x) pos->x = ll + (pos->x - lr);
    if (pos->y < lt) pos->y = lb - (lt - pos->y);
    else if(lb <= pos->y) pos->y = lt + (pos->y - lb);
}

Vec2 vec2Random(int ll, int lr, int lt, int lb) {
    Vec2 v = {RANDOM(ll, lr), RANDOM(lt, lb)};
    return v;
}

int vec2Equal(Vec2 a, Vec2 b) {
    return a.x == b.x && a.y == b.y;
}
