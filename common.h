#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>

#define SNAKE_MAX_LENGTH    128
#define SNAKE_SYMBOL        'O'
#define ITEM_SYMBOL         '#'

#define HELP_TEXT "\
ARGUMENTS:\n\
    --dimensions <WIDTH>x<HEIGHT>   Set dimensions of game to WIDTH characters\n\
                                    and HEIGHT lines. \n\
    --delay <MILISECONDS>           Set speed of the game (lower number higher\n\
                                    speed).\n\
    --agent0 <STRENGTH>             Use agent for red player and set strength.\n\
    --agent1 <STRENGTH>             Use agent for green player.\n\
    --seed <SEED>                   Set seed for srand()\n\
    --log                           Print logging information to stderr \n\
\n\
STRENGTH should be a number between 3 and 5. Higher number -> more lag. \n\
If you use --log you can pipe stderr to a file and watch it with 'tail -f' \n\
"

#endif
