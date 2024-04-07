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


static void shuffleMoveOrder(const Move *moves, Move *newOrder) {
    for (int i=0; i<3; i++) {
        newOrder[i] = moves[i];
    }
    Move m;
    for (int i=0; i<3; i++) {
        int j=rand()%3;
        m = newOrder[i];
        newOrder[i] = newOrder[j];
        newOrder[j] = m;
    }
}

static int evaluate(const Game *game, int player) {
    int enemy = (player + 1) % 2;
    int playerAlive = game->snake[player].alive;
    int enemyAlive = game->snake[enemy].alive;

    if (!playerAlive) {
        return enemyAlive ? INT_MIN + 1 : INT_MIN+1000;
    }
    if (playerAlive && !enemyAlive) {
        return INT_MAX;
    }
    
    const Snake *snake = &game->snake[player];

    const Game *prevGame = NULL;
    if (game->copyOf != NULL && game->copyOf->copyOf != NULL) {
        prevGame = game->copyOf->copyOf;
    }

    if (prevGame != NULL && snake->length > prevGame->snake[player].length) {
        return INT_MAX - 100;
    }

    return game->iteration;
}

static Game copyGame(const Game *game) {
    Game copy = *game;
    copy.copyOf = game;

    return copy;
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
            passable[XY2IDX(x, y)] = gamePositionFree(game, x, y);
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
        /* little reorder of the neighbours for more interesting paths */
        int idx1 = rand() % 4;
        int idx2 = rand() % 4;
        int b = neighbours[idx1];
        neighbours[idx1] = neighbours[idx2];
        neighbours[idx2] = b;
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
    logMsg("  before: %d, pos: %d\n", before, pos);
    logMsg("  width: %d\n", width);
    logMsg("  diff: %d\n", diff);
    logMsg("  Returning DOWN\n");
    return DOWN;
    
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
static int negamax(int player, int depth, const Game *game, Move *bestMove, int rDepth) {
    /* the snakes in the Game object are indexed with 0 and 1,
     * the negamax function needs players to be -1 and 1.
     */
    Game localGame = copyGame(game);
    if (depth % 2 == 0) {
        gameUpdate(&localGame);
    }

    int playerIdx = PLAYER_NEGAMAX_TO_IDX(player);
    int enemyIdx = (playerIdx+1)%2;
    if (depth == 0 || game->state == GAME_OVER) {
        return evaluate(game, playerIdx);
    }
    int maxScore = INT_MIN;

    Move randomMoveOrder[3];
    shuffleMoveOrder(moveOrder, randomMoveOrder);

    for (int i=0; i<3; i++) { /* own move id */
        Move playerMove = randomMoveOrder[i];


        /* make move and update local game state */
        localGame.playerInput[playerIdx] = (localGame.snake[playerIdx].dir + playerMove) % 4;

        /* next players turn */
        int score = -negamax(-player, depth-1, &localGame, bestMove, rDepth+1);

        if (score > maxScore) {
            maxScore = score;
            if (rDepth == 0) {
                *bestMove = playerMove;
            }
        }
    }
    return maxScore;
}

void agentMakeMove(Game *game, int playerIdx, int strength) {
    Move bestMove = MOVE_FORWARD;
    int player = PLAYER_IDX_TO_NEGAMAX(playerIdx);
    int lookahead = strength*2 + 1;

    /* If the enemy is near try to make a clever move */
    if (vec2Dist(game->snake[0].body[0], game->snake[1].body[0]) < lookahead) {
        logMsg("%d near enemy...\n", playerIdx);
        int score = negamax(player, lookahead, game, &bestMove, 0);
        logMsg("  score: %d\n", score);
        logMsg("  move: %d\n", bestMove);
        game->playerInput[playerIdx] = (game->snake[playerIdx].dir + bestMove) % 4;
        return;
    }
    /* otherwise try to find a way to the item if it's alive */
    if (itemAlive(&game->item)) {
        logMsg("%d pathfinding...\n", playerIdx);
        Snake *snake = &game->snake[playerIdx];
        Direction dir = bfs(game, snake->body[0], game->item.pos);
        if (dir != -1) {
            logMsg("  found path\n");
            logMsg("  new dir: %d\n", dir);
            game->playerInput[playerIdx] = dir;
            return;
        }
    }
    /* otherwise try not to die */
    logMsg("%d don't die...\n", playerIdx);
    int score = negamax(player, lookahead, game, &bestMove, 0);
    logMsg("  score: %d\n", score);
    logMsg("  move: %d\n", bestMove);
    game->playerInput[playerIdx] = (game->snake[playerIdx].dir + bestMove) % 4;
    return;
}
