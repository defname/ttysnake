#include <curses.h>
#include <stdlib.h>
#include <time.h>

#include "helper.h"
#include "snake.h"
#include "item.h"
#include "gui.h"
#include "game.h"

typedef enum {
    RUNNING,
    MENU
} GameState;

int main() {

    /* init curses */
    initscr();
   
    keypad(stdscr, TRUE);
    savetty();
    cbreak();
    noecho();
    timeout(0);
    leaveok(stdscr, TRUE);
    curs_set(0);

    srand(time(NULL));

    GameState state = MENU;

    /* screen dimensions (updated every iteration) */
    int height;
    int width;
    getmaxyx(stdscr, height, width);

    int exit = 0;

    Game game;
    gameInit(&game, width, height);

    int itemDropDelay = 0;

    for (; !exit; game.iteration++) {
        getmaxyx(stdscr, height, width);
        
        switch (state) {
            case MENU: {
                Snake *snake = game.winner == -1 ? &game.snake[0] : &game.snake[game.winner];
                
                if (game.iteration % (rand()%50+1) == 0) {
                    snake->dir += rand()%2 ? -1 : 1;
                    snake->dir %= 4;
                }

                snakeMove(snake, width, height);
                printMainMenu(width, height);
                snakeDraw(snake);
                napms(100);
                int k = wgetch(stdscr);
                if (k == ' ') {
                    gameInit(&game, width, height);
                    state = RUNNING;
                } else if (k == 'q') {
                    exit = 1;
                }
                break;
            }
            case RUNNING: {
                /* update game state */
                snakeMove(&game.snake[0], width, height);
                snakeMove(&game.snake[1], width, height);

                /* check collisions */
                if (snakeCheckCollision(&game.snake[0], &game.snake[1])) {
                    state = MENU;
                    game.winner = FIX_WINNER(game, 1);
                }
                if (snakeCheckCollision(&game.snake[1], &game.snake[0])) {
                    state = MENU;
                    game.winner = FIX_WINNER(game, 0);
                }
                /* check item */
                if (itemAlive(&game.item)) {
                    if (vec2Equal(game.snake[0].body[0], game.item.pos)) {
                        game.snake[0].length++;
                        game.item.lifespan = 0;
                    }
                    else if (vec2Equal(game.snake[1].body[0], game.item.pos)) {
                        game.snake[1].length++;
                        game.item.lifespan = 0;
                    }
                    game.item.age++;
                }
                else if (itemDropDelay > 0) itemDropDelay--;
                else {
                    itemDropDelay = 1;
                    itemInit(&game.item, 100, vec2Random(1, width-1, 1, height-1));
                }

                /* output */
                printBorder();
                //mvprintw(1, 1, "%d", game.snake[0].length);
                //mvprintw(2, 1, "%d/%d", game.snake[0].body[0].x, snake.body[0].y);

                snakeDraw(&game.snake[0]);
                snakeDraw(&game.snake[1]);

                if (itemAlive(&game.item)) itemDraw(&game.item);

                // keyPressed = wgetch(stdscr);
                napms(100);
                for (int k = wgetch(stdscr); k != ERR; k = getch()) {
                    switch (k) {
                        case 'q': state = MENU; break;
                        case KEY_LEFT:
                            snakeChangeDirection(&game.snake[0], LEFT);
                            break;
                        case KEY_RIGHT:
                            snakeChangeDirection(&game.snake[0], RIGHT);
                            break;
                        case KEY_UP:
                            snakeChangeDirection(&game.snake[0], UP);
                            break;
                        case KEY_DOWN:
                            snakeChangeDirection(&game.snake[0], DOWN);
                            break;
                        case 'a':
                            snakeChangeDirection(&game.snake[1], LEFT);
                            break;
                        case 'd':
                            snakeChangeDirection(&game.snake[1], RIGHT);
                            break;
                        case 'w':
                            snakeChangeDirection(&game.snake[1], UP);
                            break;
                        case 's':
                            snakeChangeDirection(&game.snake[1], DOWN);
                            break;
                    }
                }
                break;
            }
        }
        
        erase();
    }
    clear();
    endwin();

    printf("winner: %d\n", game.winner);
    printf("snake 0: %d %d\n", game.snake[0].length, game.snake[0].dir);
    printf("snake 1: %d %d\n", game.snake[1].length, game.snake[1].dir);
    
    return 0;
}
