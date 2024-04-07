#include <curses.h>
#include "game.h"
#include "gui.h"

void gameInit(Game *game, int *scrWidth, int *scrHeight) {
    game->screenWidth = scrWidth;
    game->screenHeight = scrHeight;

    game->state = GAME_INITIALIZED;
    game->iteration = 0;
    game->itemDropDelay = 0;
    game->winner = GAME_WINNER_UNDEF;

    game->copyOf = NULL;

    game->playerInput[0] = LEFT;
    game->playerInput[1] =  RIGHT;

    itemInit(&game->item, 0, vec2Init(10, 10));

    int centerY = *game->screenHeight /2;
    snakeInit(&game->snake[0], 0, 5, *game->screenWidth-6, centerY, LEFT);
    snakeInit(&game->snake[1], 1, 5, 6, centerY, RIGHT);
}

void gameProcessInput(Game *game) {
//    for (int k = getch(); k != ERR; k = getch()) {
    int k = getch();
        switch (k) {
            case 'q':
                game->state = GAME_EXIT;
                logMsg("Game quit by user\n");
                break;
            case ' ':
                game->state = GAME_PAUSED;
                break;
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
        }
//    }

}

int gamePositionFree(Game *game, int x, int y) {
    if (x <= 0 || x >= *game->screenWidth-1) return 0;
    if (y <= 0 || y >= *game->screenHeight-1) return 0;
    for (int pIdx=0; pIdx<2; pIdx++) {
        Snake *snake = &game->snake[pIdx];
        for (int i=0; i<snake->length; i++) {
            if (snake->body[i].x == x && snake->body[i].y == y) return 0;
        }
    }
    return 1;
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
        game->state = GAME_OVER;
        game->snake[0].alive = 0;
    }
    if (snakeCheckCollision(&game->snake[1], &game->snake[0], width, height)) {
        game->state = GAME_OVER;
        game->snake[1].alive = 0;
    }

    /* check if there's a winner */
    if (game->state == GAME_OVER) {
        if (game->snake[0].alive == 0 && game->snake[1].alive == 0) {
            game->winner = GAME_WINNER_DRAW;
        }
        else if (game->snake[0].alive == 1 && game->snake[1].alive == 0) {
            game->winner = 0;
        }
        else if (game->snake[0].alive == 0 && game->snake[1].alive == 1) {
            game->winner = 1;
        }
        else {
            game->winner = GAME_WINNER_ERROR; 
        }
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
        Position newItemPos = vec2Random(1, width-1, 1, height-1);
        while (!gamePositionFree(game, newItemPos.x, newItemPos.y)) {
            newItemPos = vec2Random(1, width-1, 1, height-1);
        }
        itemInit(&game->item, 100, newItemPos);
    }
    
    game->iteration++;
}

void gameDraw(Game *game) {
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
