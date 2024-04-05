#include <curses.h>
#include "gui.h"

#define LOGO_HEIGHT 8
#define LOGO_WIDTH  72
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
    int offsetX = (width - menuWidth) / 2;

    mvprintw(offsetY+1, offsetX, "Start Game   <space>");
    mvprintw(offsetY+3, offsetX, "Quit            q   ");
}

void printBorder() {
    SET_COLOR(COLOR_BORDER);
    wborder(stdscr, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
    UNSET_COLOR(COLOR_BORDER);
}

