# ttysnake - Two player snake for the terminal

## Usage
Compile it with ``make`` and run it with ``./ttysnake``. Make sure you have ncurses installed.
The snakes are controlled with the arroy keys and ``w``, ``a``, ``s``, ``d``. ``q`` quits the game.

```
USAGE: ./ttysnake [arguments...]
ARGUMENTS:
    --dimensions <WIDTH>x<HEIGHT>   Set dimensions of game to WIDTH characters
                                    and HEIGHT lines. 
    --delay <MILISECONDS>           Set speed of the game (lower number higher
                                    speed).
    --agent0 <STRENGTH>             Use agent for red player and set strength.
    --agent1 <STRENGTH>             Use agent for green player.
    --seed <SEED>                   Set seed for srand()
    --log                           Print logging information to stderr 

STRENGTH should be a number between 3 and 5. Higher number -> more lag. 
If you use --log you can pipe stderr to a file and watch it with 'tail -f'
```
