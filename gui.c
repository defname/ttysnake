#include <curses.h>

void printMainMenu() {
    
}

void printBorder() {
    wborder(stdscr, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
}

void printChar(int x, int y, char c) {
    mvaddch(y, x, c);
}
