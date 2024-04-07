#include <string.h>
#include <stdarg.h>
#include <time.h>
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

void parseArgs(Settings *settings, int argc, const char *argv[]) {
    /* init settings */
    settings->flags = 0;
    settings->width = 0;
    settings->height = 0;
    settings->seed = time(NULL);

    for (int i=1; i<argc; i++) {
        if (strcmp(argv[i] , "--agent0") == 0) {
            settings->flags |= FLAG_AGENT_0;
            continue;
        }
        if (strcmp(argv[i] , "--agent1") == 0) {
            settings->flags |= FLAG_AGENT_1;
            continue;
        }
        if (strcmp(argv[i], "--dimension") == 0 && argc > i+1) {
            int w, h;
            if (sscanf(argv[++i], "%dx%d", &w, &h) == 2) {
                settings->flags |= FLAG_FIXED_SIZE;
                settings->width = w;
                settings->height = h;
                continue;
            }
        }
        if (strcmp(argv[i], "--seed") == 0 && argc > i+1) {
            int seed;
            if (sscanf(argv[++i], "%d", &seed) == 1) {
                settings->seed = seed;
                continue;
            }
        }
        if (strcmp(argv[i], "--log") == 0) {
            settings->flags |= FLAG_LOG;
            continue;
        }
        printf("Usage: %s [--agent0] [--agent1] [--log] [--dimension <width>x<height>] [--seed <seed>]\n", argv[0]);
        exit(EXIT_FAILURE);

    }

}

void logMsg(const char *fmt, ...) {
    if (!(settings.flags & FLAG_LOG)) return;
    va_list args;
    va_start(args, fmt);
    int rc = vfprintf(stderr, fmt, args);
    va_end(args);
}
