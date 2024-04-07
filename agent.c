#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <curses.h>
#include "agent.h"
#include "game.h"
#include "item.h"
#include "snake.h"

typedef enum {
    MOVE_LEFT = -1,
    MOVE_FORWARD = 0,
    MOVE_RIGHT = 1
} Move;

Move moveOrder[3] = {MOVE_FORWARD, MOVE_LEFT, MOVE_RIGHT};

#define PLAYER_IDX_TO_NEGAMAX(playerIdx) ((playerIdx) == 0 ? -1 : 1)
#define PLAYER_NEGAMAX_TO_IDX(player)    ((player) == -1 ? 0 : 1)

void agentInit(Agent *agent, Game *game) {
    agent->game = game;
}

static int evaluate(const Game *game, const Game *prevGame, int player) {
    int enemy = (player + 1) % 2;
    int playerAlive = game->snake[player].alive;
    int enemyAlive = game->snake[enemy].alive;

    if (!playerAlive) {
        return enemyAlive ? -1000000 : -5000;
    }
    if (playerAlive && !enemyAlive) {
        return 10000000;

    }
    
    const Snake *snake = &game->snake[player];

    if (prevGame != NULL && snake->length > prevGame->snake[player].length) {
        return 10000;
    }
    return game->iteration;
}

static Game copyGame(const Game *game) {
    Game copy = *game;

    return copy;
}

static int isPassable(Game *game, int x, int y) {
    if (x <= 0 || x >= *game->screenWidth-1) return 0;
    if (y <= 0 || y >= *game->screenHeight-1) return 0;
    for (int pIdx=0; pIdx<2; pIdx++) {
        Snake *snake = &game->snake[pIdx];
        for (int i=0; i<snake->length; i++) {
            if (snake->body[i].x == x && snake->body[i].y == y) return 0;
        }
    }
    return 1;
}

/**
 * Perform a breadth-first-search and return the direction to choose.
 * Return -1 if no path is found
 */
static int bfs(Game *game, Position startPos, Position endPos) {
#define XY2IDX(x, y)    ((y)*width+x)
#define POS2IDX(pos)    XY2IDX(pos.x, pos.y)
#define IDX2X(idx)      ((idx)%width)
#define IDX2Y(idx)      (((idx)-IDX2X(idx))/width)
#define QUEUE_EMPTY()   (queueFront == queueRear)
#define MAX 10000
#define INF INT_MAX

    int width = *game->screenWidth;
    int height = *game->screenHeight;
    int maxIdx = width * height;

    int start = POS2IDX(startPos);
    int end = POS2IDX(endPos);

    /* map every idx to 1 (done) or 0 */
    int done[MAX];

    /* queue is implemented as a simple circular queue.
     * It grows in opposite direction, rear will grow. */
    int queue[MAX];
    queue[0] = start;
    int queueFront = 0;
    int queueRear = 1;
    
    /* for passable positions 1, otherwise 0 */
    int passable[MAX];
    
    /* distances between two indices */
    int dist[MAX];

    /* the path to remember: cameFrom[to] = from */
    int cameFrom[MAX];
    cameFrom[start] = -1;

    /* initialize lists */
    for (int y=0; y<height; y++) {
        for (int x=0; x<width; x++) {
            passable[XY2IDX(x, y)] = isPassable(game, x, y);
        }
    }
    for (int i=0; i<MAX; i++) {
        dist[i] = INF;
        done[i] = 0;
    }

    dist[start] = 0;

    int success = 0;

    while (!QUEUE_EMPTY()) {
        /* dequeue */
        int current = queue[queueFront];
        queueFront = (queueFront+1) % MAX;

        /* if target was found exit loop */
        if (current == end) {
            success = 1;
            break;
        }

        /* if position was already visited continue */
        if (done[current] == 1) continue;

        /* mark position as visited */
        done[current] = 1;

        /* check neighbours */
        /* right, left, bottom, top */
        int neighbours[4] = {current+1, current-1, current+width, current-width};
        for (int i=0; i<4; i++) {
            int next = neighbours[i];
            
            /* if position is occupied continue */
            if (!passable[next]) {
                continue;
            }
            
            int newDist = dist[current]+1;
            /* if there is already a better route continue */
            if (dist[next] < newDist) continue;
            dist[next] = newDist;
            cameFrom[next] = current;

            queue[queueRear] = next;
            queueRear = (queueRear+1) % MAX;
        }

    }


    if (!success) {
        return -1;
    }

    /* find first position of path */
    int pos = end;
    int before = cameFrom[pos];
    while (cameFrom[pos] != -1) {
        before = pos;
        pos = cameFrom[pos];
    }

    int diff = before - pos;
    if (diff == 1) return RIGHT;
    if (diff == -1) return LEFT;
    if (diff == width) return DOWN;
    if (diff == -width) return UP;
 
    logMsg("Should not happen\n");
    logMsg("width: %d\n", width);
    logMsg("diff: %d\n", diff);
    exit(123);
    
#undef INF
#undef MAX
#undef IDX2Y
#undef IDX2X
#undef POS2IDX
#undef XY2IDX
}

/**
 * Classic negamax algorithm to calculate the best move.
 *
 * @param player -1 or 1 (different than the indices of the snakes!)
 *               You can use the macros PLAYER_IDX_TO_NEGAMAX, and
 *               PLAYER_NEGAMAX_TO_IDX to calculate them.
 * @param depth  The maximum recursion depth of the algorithm
 * @param game   Pointer to the game object. This object will not be modified.
 * @param bestMove pointer to a Move. The best move will be stored there.
 * @param rDepth has to be 0!
 * @param prevGame has to be NULL
 * @return The score for the player.
 */
static int negamax(int player, int depth, const Game *game, Move *bestMove, int rDepth, const Game *prevGame) {
    /* the snakes in the Game object are indexed with 0 and 1,
     * the negamax function needs players to be -1 and 1.
     */
    Game localGame = copyGame(game);
    if (depth % 2 == 0) {
        gameUpdate(&localGame);
    }

    int playerIdx = PLAYER_NEGAMAX_TO_IDX(player);
    int enemyIdx = (playerIdx+1)%2;
    if (depth == 0 || !game->running) {
        return evaluate(game, prevGame, playerIdx);
    }
    int maxScore = INT_MIN;

    for (int i=0; i<3; i++) { /* own move id */
        Move playerMove = moveOrder[i];


        /* make move and update local game state */
        localGame.playerInput[playerIdx] = (localGame.snake[playerIdx].dir + playerMove) % 4;

        /* next players turn */
        int score = -negamax(-player, depth-1, &localGame, bestMove, rDepth+1, game);

        if (score > maxScore) {
            maxScore = score;
            if (rDepth == 0) {
                *bestMove = playerMove;
            }
        }
    }
    return maxScore;
}

void agentMakeMove(Agent *agent, int playerIdx) {
    Move bestMove = MOVE_FORWARD;
    int player = PLAYER_IDX_TO_NEGAMAX(playerIdx);

    /* If the enemy is near try to make a clever move */
    if (vec2Dist(agent->game->snake[0].body[0], agent->game->snake[1].body[0]) < 10) {
        logMsg("%d near enemy...\n", playerIdx);
        int score = negamax(player, 5, agent->game, &bestMove, 0, NULL);
        logMsg("  score: %d\n", score);
        logMsg("  move: %d\n", bestMove);
        agent->game->playerInput[playerIdx] = (agent->game->snake[playerIdx].dir + bestMove) % 4;
        return;
    }
    /* otherwise try to find a way to the item if it's alive */
    if (itemAlive(&agent->game->item)) {
        logMsg("%d pathfinding...\n", playerIdx);
        Snake *snake = &agent->game->snake[playerIdx];
        Direction dir = bfs(agent->game, snake->body[0], agent->game->item.pos);
        if (dir != -1) {
            logMsg("  found path\n");
            logMsg("  new dir: %d\n", dir);
            agent->game->playerInput[playerIdx] = dir;
            return;
        }
    }
    /* otherwise try not to die */
    logMsg("%d don't die...\n", playerIdx);
    int score = negamax(player, 5, agent->game, &bestMove, 0, NULL);
    logMsg("  score: %d\n", score);
    logMsg("  move: %d\n", bestMove);
    agent->game->playerInput[playerIdx] = (agent->game->snake[playerIdx].dir + bestMove) % 4;
    return;
}
