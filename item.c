#include <curses.h>
#include "item.h"
#include "gui.h"
#include "common.h"

void itemInit(Item *item, int lifespan, Position pos) {
    item->age = 0;
    item->lifespan = lifespan;
    item->pos = pos;
}

void itemDraw(Item *item) {
    if (item->age >= item->lifespan) return;
    SET_COLOR(COLOR_ITEM);
    mvaddch(item->pos.y, item->pos.x, ITEM_SYMBOL);
    UNSET_COLOR(COLOR_ITEM);
}

int itemAlive(Item *item) {
    return item->age < item->lifespan;
}
