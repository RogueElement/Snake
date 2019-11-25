# Snake

## Game
This is a TUI clone of the classic snake game.

Rules of the game:
- The snake starts at the center of the board, moving north (upward).
- The snake moves at a constant speed (once every 100 milliseconds).
- The snake moves only north, south, east, or west.
- Food (represented with `A`) appear at random locations, and persist for a random amount of time.
- There is always exactly one food visible at any given time.
- When the snake eats a food item, it gets longer and the game score goes up.
- The game continues until the snake dies.
- Death occurs either by the snake head:
    - running into a wall (boundary of the screen).
    - running into its tail.

Game controls:
- The arrow keys change the snake's direction.
- `P` pauses and unpauses the game.
- `Q` will quit the game instantly.
- `R` will reset the snake and game score if the snake is dead.

The game board is initialized to the full width and height of the terminal window when it launches. Resizing the window will mess up the TUI but the game will continue as is;

There are a few hidden features in the game.

## Building
This project requires `ncurses`<sup>1</sup>  and a c compiler to build.
Building and running has only been tested on `Ubuntu 18` and `Arch`, but should work on any platform that can get `ncurses`.

The makefile uses `g++` and makes an executable `snake` in the current directory.

<sup>1</sup> On `Ubuntu 18` the packages required were `libncurses5-dev` and `libncursesw5-dev`