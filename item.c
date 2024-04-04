#include <curses.h>
#include "item.h"
#include "gui.h"

void itemInit(Item *item, int lifespan, Position pos) {
    item->age = 0;
    item->lifespan = lifespan;
    item->pos = pos;
}

void itemDraw(Item *item) {
    if (item->age >= item->lifespan) return;
    printChar(item->pos.x, item->pos.y, 'X');
}

int itemAlive(Item *item) {
    return item->age < item->lifespan;
}
