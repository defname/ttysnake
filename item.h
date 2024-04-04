#ifndef ITEM_H
#define ITEM_H

#include "helper.h"

typedef struct {
    int age;
    int lifespan;
    Position pos;
} Item;

void itemInit(Item *item, int lifespan, Position pos);
void itemDraw(Item *item);
int itemAlive(Item *item);

#endif
