/**
 * ttysnake - a simple two player snake game
 *
 * To compile it you need the ncurses library (or an replacement) installed.
 * Compile it with
 *   gcc -o ttysnake *.c -lcurses
 * 
 * The snake are controlled with the arrow keys and "w", "a", "s", "g".
 * "q" quits the game.
 *
 * http://github.com/naarf/ttysnake
 */

#include <curses.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "snake.h"
#include "gui.h"
#include "game.h"
#include "agent.h"

Settings settings;

int main(int argc, const char *argv[]) {
    parseArgs(&settings, argc, argv);

    /* init curses */
    initscr();
   
    keypad(stdscr, TRUE);
    savetty();
    cbreak();
    noecho();
    timeout(0);
    leaveok(stdscr, TRUE);
    curs_set(0);

    logMsg("Seed: %d\n", settings.seed);
    srand(settings.seed);

    /* init colors */
    if (has_colors()) {
        start_color();
        init_pair(COLOR_SNAKE0, COLOR_RED, COLOR_BLACK);
        init_pair(COLOR_SNAKE1, COLOR_GREEN, COLOR_BLACK);
        init_pair(COLOR_DRAW, COLOR_YELLOW, COLOR_BLACK);
        init_pair(COLOR_ITEM, COLOR_YELLOW, COLOR_BLACK);
        init_pair(COLOR_LOGO, COLOR_BLUE, COLOR_BLACK);
    }

    /* screen dimensions */
    int height;
    int width;
    if (settings.flags & FLAG_FIXED_SIZE) {
        width = settings.width;
        height = settings.height;
    }
    else {
       getmaxyx(stdscr, height, width);
    }

    int exit = 0;
    
    /* used to determine the first iteration of game.running = 0 */
    int gameJustStopped = 0;

    Game game;
    gameInit(&game, &width, &height);

    Game backgroundGame;
    gameInit(&backgroundGame, &width, &height);
    backgroundGame.state = GAME_RUNNING;
    int backgroundGameDelay = 100;


    for (; !exit;) {
        /* update screen dimensions */
        if (!(settings.flags & FLAG_FIXED_SIZE)) {
            getmaxyx(stdscr, height, width);
        }
    
        switch (game.state) { /* show menu */
            case GAME_INITIALIZED:
            case GAME_EXIT: {
                if (gameJustStopped == 1) {
                    logMsg("winner: %d\n", game.winner);
                    logMsg("alive?  snake0: %d snake1: %d\n", game.snake[0].alive, game.snake[1].alive);
                    logMsg("dir?    snake0: %d snake1: %d\n\n", game.snake[0].dir, game.snake[1].dir);
                    gameJustStopped = 0;
                }
                
                /* draw stuff */
                printMainMenu(width, height);

                if (backgroundGame.state == GAME_RUNNING) {
                    backgroundGameDelay = 40;

                    gameUpdate(&backgroundGame);
                    gameDraw(&backgroundGame);

                    agentMakeMove(&backgroundGame, 0, 3);
                    agentMakeMove(&backgroundGame, 1, 3);
                }
                else {
                    gameDraw(&backgroundGame);
                    backgroundGameDelay--;
                    if (backgroundGameDelay == 0) {
                        gameInit(&backgroundGame, &width, &height);
                        backgroundGame.state = GAME_RUNNING;
                    }
                }

                napms(settings.delay);

                /* process input */
                int k = wgetch(stdscr);
                if (k == ' ') {
                    gameInit(&game, &width, &height);
                    game.state = GAME_RUNNING;
                } else if (k == 'q') {
                    exit = 1;
                }
                break;
            }
        
            case GAME_RUNNING: { /* game running */
                /* update game state */
                gameUpdate(&game);

                gameDraw(&game);
                
                napms(100);
                /* the input processing has to be at the end of the game loop
                 * because curses doesn't draw anything otherwise */
                gameProcessInput(&game);
                if (settings.flags & FLAG_AGENT_0) {
                    agentMakeMove(&game, 0, settings.agent0);
                }
                if (settings.flags & FLAG_AGENT_1) {
                    agentMakeMove(&game, 1, settings.agent1);
                }

                if (game.state == GAME_EXIT) {
                    gameJustStopped = 1;
                }
                break;
            }

            case GAME_PAUSED: {
                gameDraw(&game);
                printPause(width, height);
                napms(settings.delay);
                int k = getch();
                if (k == ' ') {
                    game.state = GAME_RUNNING;
                    continue;
                }
                break;
            }

            case GAME_OVER: {
                gameDraw(&game);
                printGameOver(&game, width, height);
                napms(100);
                int k = getch();
                if (k == ' ') {
                    game.state = GAME_EXIT;
                    continue;
                }
                break;
            }
        }
        
        erase();
    }
    clear();
    endwin();

    return EXIT_SUCCESS;
}
