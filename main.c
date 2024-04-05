#include <curses.h>
#include <stdlib.h>
#include <time.h>

#include "helper.h"
#include "snake.h"
#include "item.h"
#include "gui.h"

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

    /* number of game loop iterations */
    int iteration;
    /* exit condition */
    int exit = 0;
    /* screen dimensions (updated every iteration) */
    int height;
    int width;

    int winner = 0;

    int itemDropDelay = 0;

    Item item;
    itemInit(&item, 0, vec2Init(10, 10));

    Snake snake1;
    snakeInit(&snake1, 5, 10, 4, RIGHT);

    Snake snake2;
    snakeInit(&snake2, 5, 30, 60, LEFT);

    for (iteration = 0; !exit; iteration++) {
        getmaxyx(stdscr, height, width);
        
        switch (state) {
            case MENU:
                snake2.length = 0;
                
                if (iteration % (rand()%50+1) == 0) {
                    snake1.dir += rand()%2 - 1;
                    snake1.dir %= 4;
                }

                snakeMove(&snake1, width, height);
                printMainMenu(width, height);
                snakeDraw(&snake1);
                napms(100);
                int k = wgetch(stdscr);
                if (k == ' ') {
                    snakeInit(&snake1, 5, 10, 4, RIGHT);
                    snakeInit(&snake2, 5, 30, 20, LEFT);
                    item.lifespan = 0;
                    winner = 0;
                    iteration = 0;
                    state = RUNNING;
                } else if (k == 'q') {
                    exit = 1;
                }
                break;
            case RUNNING: {
                /* update game state */
                snakeMove(&snake1, width, height);
                snakeMove(&snake2, width, height);

                /* check collisions */
                if (snakeCheckCollision(&snake1, &snake2)) {
                    state = MENU;
                    winner = FIX_WINNER(snake1, snake2, 2);
                }
                if (snakeCheckCollision(&snake2, &snake1)) {
                    state = MENU;
                    winner = FIX_WINNER(snake1, snake2, 1);
                }
                /* check item */
                if (itemAlive(&item)) {
                    if (vec2Equal(snake1.body[0], item.pos)) {
                        snake1.length++;
                        item.lifespan = 0;
                    }
                    else if (vec2Equal(snake2.body[0], item.pos)) {
                        snake2.length++;
                        item.lifespan = 0;
                    }
                    item.age++;
                }
                else if (itemDropDelay > 0) itemDropDelay--;
                else {
                    itemDropDelay = 1;
                    itemInit(&item, 100, vec2Random(1, width-1, 1, height-1));
                }

                /* output */
                printBorder();
                //mvprintw(1, 1, "%d", snake1.length);
                //mvprintw(2, 1, "%d/%d", snake1.body[0].x, snake.body[0].y);

                snakeDraw(&snake1);
                snakeDraw(&snake2);

                if (itemAlive(&item)) itemDraw(&item);

                // keyPressed = wgetch(stdscr);
                napms(100);
                for (int k = wgetch(stdscr); k != ERR; k = getch()) {
                    switch (k) {
                        case 'q': state = MENU; break;
                        case KEY_LEFT:
                            snakeChangeDirection(&snake1, LEFT);
                            break;
                        case KEY_RIGHT:
                            snakeChangeDirection(&snake1, RIGHT);
                            break;
                        case KEY_UP:
                            snakeChangeDirection(&snake1, UP);
                            break;
                        case KEY_DOWN:
                            snakeChangeDirection(&snake1, DOWN);
                            break;
                        case 'a':
                            snakeChangeDirection(&snake2, LEFT);
                            break;
                        case 'd':
                            snakeChangeDirection(&snake2, RIGHT);
                            break;
                        case 'w':
                            snakeChangeDirection(&snake2, UP);
                            break;
                        case 's':
                            snakeChangeDirection(&snake2, DOWN);
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

    printf("winner: %d\n", winner);
    printf("snake1: %d %d\n", snake1.length, snake1.dir);
    printf("snake2: %d %d\n", snake2.length, snake2.dir);
    
    return 0;
}
