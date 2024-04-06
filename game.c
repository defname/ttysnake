#include <curses.h>
#include "game.h"
#include "gui.h"

void gameInit(Game *game, int *scrWidth, int *scrHeight) {
    game->screenWidth = scrWidth;
    game->screenHeight = scrHeight;

    game->running = 0;
    game->iteration = 0;
    game->itemDropDelay = 0;
    game->winner = -1;

    game->playerInput[0] = LEFT;
    game->playerInput[1] =  RIGHT;

    itemInit(&game->item, 0, vec2Init(10, 10));

    int centerY = *game->screenHeight /2;
    snakeInit(&game->snake[0], 5, *game->screenWidth-6, centerY, LEFT);
    snakeInit(&game->snake[1], 5, 6, centerY, RIGHT);
}

void gameProcessInput(Game *game) {
//    for (int k = wgetch(stdscr); k != ERR; k = getch()) {
    int k = getch();
        switch (k) {
            case 'q':
                game->running = 0;
                fprintf(stderr, "Game quit by user\n");
                break;
    /*
            case KEY_LEFT:
                game->playerInput[0] = LEFT;
                break;
            case KEY_RIGHT:
                game->playerInput[0] = RIGHT;
                break;
            case KEY_UP:
                game->playerInput[0] = UP;
                break;
            case KEY_DOWN:
                game->playerInput[0] = DOWN;
                break;
            case 'a':
                game->playerInput[1] = LEFT;
                break;
            case 'd':
                game->playerInput[1] = RIGHT;
                break;
            case 'w':
                game->playerInput[1] = UP;
                break;
            case 's':
                game->playerInput[1] = DOWN;
                break;
    */
        }
//    }

}

void gameUpdate(Game *game) {
    int width = *game->screenWidth;
    int height = *game->screenHeight;

    /* update snake directions */
    snakeChangeDirection(&game->snake[0], game->playerInput[0]);
    snakeChangeDirection(&game->snake[1], game->playerInput[1]);

    /* move snakes */
    snakeMove(&game->snake[0], width, height);
    snakeMove(&game->snake[1], width, height);

    /* check collisions */
    if (snakeCheckCollision(&game->snake[0], &game->snake[1], width, height)) {
        game->winner = FIX_WINNER(game, 1);
        game->running = 0;
        game->snake[0].alive = 0;
    }
    if (snakeCheckCollision(&game->snake[1], &game->snake[0], width, height)) {
        game->winner = FIX_WINNER(game, 0);
        game->running = 0;
        game->snake[1].alive = 0;
    }
    /* check item */
    if (itemAlive(&game->item)) {
        if (vec2Equal(game->snake[0].body[0], game->item.pos)) {
            game->snake[0].length++;
            game->item.lifespan = 0;
        }
        else if (vec2Equal(game->snake[1].body[0], game->item.pos)) {
            game->snake[1].length++;
            game->item.lifespan = 0;
        }
        game->item.age++;
    }
    else if (game->itemDropDelay > 0) game->itemDropDelay--;
    else {
        game->itemDropDelay = 1;
        itemInit(&game->item, 100, vec2Random(1, width-1, 1, height-1));
    }

}

void gameDraw(Game *game) {
    /* don't draw the last frame, to prevent drawing snakes over the border*/
    if (!game->running) return;

    /* draw border */
    printBorder(*game->screenWidth, *game->screenHeight);
    //mvprintw(1, 1, "%d", game->snake[0].length);
    //mvprintw(2, 1, "%d/%d", game->snake[0].body[0].x, snake.body[0].y);

    /* draw snakes */
    SET_COLOR(COLOR_SNAKE0);
    snakeDraw(&game->snake[0]);
    UNSET_COLOR(COLOR_SNAKE0);
    SET_COLOR(COLOR_SNAKE1);
    snakeDraw(&game->snake[1]);
    UNSET_COLOR(COLOR_SNAKE1);

    /* draw item */
    if (itemAlive(&game->item)) itemDraw(&game->item);
}
