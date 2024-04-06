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

enum {
    FLAG_FIXED_SIZE     = 1,
    FLAG_AGENT_0        = 2,
    FLAG_AGENT_1        = 4
};

typedef struct {
    int flags;
    int width;
    int height;
} Settings;

#define SETTINGS_FIXED_SIZE(settings) (settings.flags & FLAG_FIXED_SIZE)

static void parseArgs(Settings *settings, int argc, const char *argv[]) {
    /* init settings */
    settings->flags = 0;
    settings->width = 0;
    settings->height = 0;

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
        printf("Usage: %s [--agent0] [--agent1] [--dimension <width>x<height>]\n", argv[0]);
        exit(EXIT_FAILURE);

    }

}

int main(int argc, const char *argv[]) {
    Settings settings;
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

    srand(time(NULL));

    /* init colors */
    if (has_colors()) {
        start_color();
        init_pair(COLOR_SNAKE0, COLOR_RED, COLOR_BLACK);
        init_pair(COLOR_SNAKE1, COLOR_GREEN, COLOR_BLACK);
        init_pair(COLOR_SNAKE_DRAW, COLOR_YELLOW, COLOR_BLACK);
        init_pair(COLOR_ITEM, COLOR_YELLOW, COLOR_BLACK);
        init_pair(COLOR_LOGO, COLOR_BLUE, COLOR_BLACK);
    }

    /* screen dimensions (updated every iteration) */
    int height;
    int width;
    if (SETTINGS_FIXED_SIZE(settings)) {
        width = settings.width;
        height = settings.height;
    }
    else {
       getmaxyx(stdscr, height, width);
    }

    int exit = 0;

    Game game;
    gameInit(&game, &width, &height);

    Agent agent;
    agentInit(&agent, &game);

    for (; !exit; game.iteration++) {
        if (!SETTINGS_FIXED_SIZE(settings)) {
            getmaxyx(stdscr, height, width);
        }
    
        if (!game.running) { /* show menu */
            fprintf(stderr, "winner: %d\n", game.winner);
            fprintf(stderr, "alive?  snake0: %d snake1: %d\n", game.snake[0].alive, game.snake[1].alive);
            fprintf(stderr, "dir?    snake0: %d snake1: %d\n\n", game.snake[0].dir, game.snake[1].dir);

            /* let the winning snake continue crawling around */
            int snakeIdx = game.winner == -1 ? 0 : game.winner;
            Snake *snake = &game.snake[snakeIdx];
            int snakeColor = snakeIdx == 0 ? COLOR_SNAKE0 : COLOR_SNAKE1;
            snakeColor = game.winner != -1 ? snakeColor : COLOR_SNAKE_DRAW;
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
            if (settings.flags & FLAG_AGENT_0) {
                agentMakeMove(&agent, 0);
            }
            if (settings.flags & FLAG_AGENT_1) {
                agentMakeMove(&agent, 1);
            }
        }
        
        erase();
    }
    clear();
    endwin();

    printf("winner: %d\n", game.winner);
    printf("snake 0: %d\n", game.snake[0].length);
    printf("snake 1: %d\n", game.snake[1].length);
    
    return 0;
}
