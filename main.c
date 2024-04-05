#include <curses.h>
#include <stdlib.h>
#include <time.h>

#include "snake.h"
#include "gui.h"
#include "game.h"

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

    /* init colors */
    if (has_colors()) {
        start_color();
        init_pair(COLOR_SNAKE0, COLOR_RED, COLOR_BLACK);
        init_pair(COLOR_SNAKE1, COLOR_GREEN, COLOR_BLACK);
        init_pair(COLOR_ITEM, COLOR_YELLOW, COLOR_BLACK);
        init_pair(COLOR_LOGO, COLOR_BLUE, COLOR_BLACK);
    }

    /* screen dimensions (updated every iteration) */
    int height;
    int width;
    getmaxyx(stdscr, height, width);

    int exit = 0;

    Game game;
    gameInit(&game, &width, &height);

    for (; !exit; game.iteration++) {
        getmaxyx(stdscr, height, width);
    
        if (!game.running) { /* show menu */
            /* let the winning snake continue crawling around */
            int snakeIdx = game.winner == -1 ? 0 : game.winner;
            Snake *snake = &game.snake[snakeIdx];
            int snakeColor = snakeIdx == 0 ? COLOR_SNAKE0 : COLOR_SNAKE1;
            /* change the direction randomly */
            if (game.iteration % (rand()%50+1) == 0) {
                snake->dir += rand()%2 ? -1 : 1;
                snake->dir %= 4;
            }
            snakeMove(snake, width, height);

            /* draw stuff */
            printMainMenu(width, height);
            SET_COLOR(snakeColor);
            snakeDraw(snake);
            UNSET_COLOR(snakeColor);

            napms(100);

            /* process input */
            int k = wgetch(stdscr);
            if (k == ' ') {
                gameInit(&game, &width, &height);
                game.running = 1;
            } else if (k == 'q') {
                exit = 1;
            }
        }
        else { /* game running */
            /* update game state */
            gameUpdate(&game);

            gameDraw(&game);
            
            napms(100);
            /* the input processing has to be at the end of the game loop
             * because curses doesn't draw anything otherwise */
            gameProcessInput(&game);
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
