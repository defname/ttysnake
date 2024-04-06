#include <curses.h>
#include "gui.h"

#define LOGO_HEIGHT 8
#define LOGO_WIDTH  72

#define BREAKPOINT_HEIGHT 22
#define BREAKPOINT_WIDTH  74


const char text[LOGO_HEIGHT][LOGO_WIDTH+1] = {
    " _______  _______  __   __  _______  __    _  _______  ___   _  _______ \0",
    "|       ||       ||  | |  ||       ||  |  | ||   _   ||   | | ||       |\0",
    "|_     _||_     _||  |_|  ||  _____||   |_| ||  |_|  ||   |_| ||    ___|\0",
    "  |   |    |   |  |       || |_____ |       ||       ||      _||   |___ \0",
    "  |   |    |   |  |_     _||_____  ||  _    ||       ||     |_ |    ___|\0",
    "  |   |    |   |    |   |   _____| || | |   ||   _   ||    _  ||   |___ \0",
    "  |___|    |___|    |___|  |_______||_|  |__||__| |__||___| |_||_______|\0"
};


void printLogo(int width, int height) {
    if (width < BREAKPOINT_WIDTH || height < BREAKPOINT_HEIGHT) {
        SET_COLOR(COLOR_LOGO);
        mvprintw(height/2-3, (width-8)/2, "TTYSNAKE");
        UNSET_COLOR(COLOR_LOGO);
    }
    int offsetX = (width - LOGO_WIDTH) / 2;
    int offsetY = (height - LOGO_HEIGHT) / 2;

    SET_COLOR(COLOR_LOGO);
    for (int y=0; y<LOGO_HEIGHT; y++) {
        mvprintw(offsetY+y, offsetX, "%*s", LOGO_WIDTH, text[y]);
    }
    UNSET_COLOR(COLOR_LOGO);
}

void printMainMenu(int width, int height) {
    printLogo(width, height);

    int menuWidth = 20;
    int offsetY = (height + LOGO_HEIGHT) / 2 + 2;
    if (height < BREAKPOINT_HEIGHT || width < BREAKPOINT_WIDTH) {
        offsetY = height/2 + 1;
    }
    int offsetX = (width - menuWidth) / 2;

    mvprintw(offsetY+1, offsetX, "Start Game   <space>");
    mvprintw(offsetY+3, offsetX, "Quit            q   ");
}

static void rectangle(int y1, int x1, int y2, int x2)
{
    mvhline(y1, x1, 0, x2-x1);
    mvhline(y2, x1, 0, x2-x1);
    mvvline(y1, x1, 0, y2-y1);
    mvvline(y1, x2, 0, y2-y1);
    mvaddch(y1, x1, ACS_ULCORNER);
    mvaddch(y2, x1, ACS_LLCORNER);
    mvaddch(y1, x2, ACS_URCORNER);
    mvaddch(y2, x2, ACS_LRCORNER);
}
void printBorder(int width, int height) {
    SET_COLOR(COLOR_BORDER);
    rectangle(0, 0, height-1, width-1);
    UNSET_COLOR(COLOR_BORDER);
}

