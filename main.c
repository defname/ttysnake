#include <curses.h>
#include "helper.h"
#include "snake.h"
#include "item.h"
#include "gui.h"

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
        /* process input */
        getmaxyx(stdscr, height, width);

        /* update game state */
        snakeMove(&snake1, width, height);
        snakeMove(&snake2, width, height);

        /* check collisions */
        if (snakeCheckCollision(&snake1, &snake2)) {
            exit = 1;
            winner = FIX_WINNER(snake1, snake2, 2);
        }
        if (snakeCheckCollision(&snake2, &snake1)) {
            exit = 1;
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
                case 'q': exit = 1; break;
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
        
        erase();
    }
    clear();
    endwin();

    printf("winner: %d\n", winner);
    printf("snake1: %d %d\n", snake1.length, snake1.dir);
    printf("snake2: %d %d\n", snake2.length, snake2.dir);
    
    return 0;
}
