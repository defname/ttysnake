#ifndef AGENT_H
#define AGENT_H

#include "game.h"

typedef struct {
    Game *game;
} Agent;

void agentInit(Agent *agent, Game *game);
void agentMakeMove(Agent *agent, int player);

#endif
