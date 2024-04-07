#ifndef GAME_H
#define GAME_H

#include "helper.h"
#include "item.h"
#include "snake.h"

#define GAME_WINNER_DRAW    -1
#define GAME_WINNER_UNDEF   -100
#define GAME_WINNER_ERROR   -123

typedef enum {
    GAME_INITIALIZED,
    GAME_RUNNING,
    GAME_PAUSED,
    GAME_OVER,
    GAME_EXIT
} GameState;

typedef struct _Game{
    /* pointer to variables holding the current screen dimensions */
    int *screenWidth;
    int *screenHeight;

    Settings *settings;

    /* number of iterations of the game loop */
    int iteration;
    /* indicator if the game is running */
    GameState state;
    /* delay to wait before a new item is dropped. not used so far */
    int itemDropDelay;
    /* 0, 1 depending on which snake won, -1 on draw */
    int winner;

    /* only one item is possible at the same time so far */
    Item item;
    /* snake objects */
    Snake snake[2];

    /* last player input */
    Direction playerInput[2];

    /* indicates that this object is a copy, used by the agent, that
     * creates copies of the game */
    const struct _Game *copyOf;
} Game;

/* if both snakes run directly against each other it's a draw */
#define FIX_WINNER(game, winner) (game->snake[0].dir == OPPOSITE_DIR(game->snake[1].dir) ? -1 : winner)

void gameInit(Game *game, int *scrWidth, int *scrHeight);
int gamePositionFree(Game *game, int x, int y);
void gameUpdate(Game *game);
void gameDraw(Game *game);
void gameProcessInput(Game *game);

#endif
