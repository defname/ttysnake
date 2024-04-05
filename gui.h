#ifndef GUI_H
#define GUI_H

#include "helper.h"

#define COLOR_SNAKE0    1
#define COLOR_SNAKE1    2
#define COLOR_SNAKE_DRAW 3
#define COLOR_ITEM      4
#define COLOR_LOGO      5
#define COLOR_BORDER    COLOR_LOGO

#define SET_COLOR(color) has_colors() && attron(COLOR_PAIR(color))
#define UNSET_COLOR(color) has_colors() && attroff(COLOR_PAIR(color))

void printLogo(int width, int height);
void printMainMenu(int width, int height);
void printBorder();

#endif
