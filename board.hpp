#include <chrono>
#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <thread>
#include <curses.h>

#include "snake.hpp"

#define WALLCOLOR 1
#define FOODCOLOR 2
#define SNAKECOLOR 3

class Board
{
public:
    // Makes a new board with 100 milliseconds between
    // each snake move
    Board();
    // Starts the main game loop
    void run();
    ~Board();

private:
    int height, width, score, multiplier;
    bool paused, dead, immortal;

    Coord food;
    Snake snake;
    WINDOW *win;

    std::vector<std::vector<chtype>> board;
    std::chrono::system_clock::time_point last_moved, food_created;
    std::chrono::milliseconds speed_time, food_time;
    std::uniform_int_distribution<int> generator;
    std::mt19937 rand_eng;

    void update_screen();
    // Generates a random Coord for food that is within the board and
    // not occupied by the snake and sets food_time to a random
    // amount of time between the time it would take the snake
    // head to get to food (x1.5) and double that time (so between x1.5
    // optimal time and x3 optimal time)
    void gen_food();
};

Board::Board()
    : speed_time(std::chrono::milliseconds(100)), paused(false), dead(false), immortal(false),
      food({0, 0}), score(0), multiplier(1)
{
    initscr();                       // Initialize ncurses terminal window
    getmaxyx(stdscr, height, width); // Get the height/width of the terminal window
    win = stdscr;
    cbreak();           // Disable line buffering
    noecho();           // Don't echo key presses to the terminal
    nodelay(win, true); // Enable non blocking input capture
    keypad(win, true);  // Capture arrow keys as input
    curs_set(0);        // Set the cursor to invisible
    start_color();      // Enable colors

    snake.begin(height / 2, width / 2); // Start the snake in the middle of the screen

    init_pair(WALLCOLOR, COLOR_RED, COLOR_RED);
    init_pair(FOODCOLOR, COLOR_GREEN, COLOR_MAGENTA);
    init_pair(SNAKECOLOR, COLOR_BLUE, COLOR_BLACK);

    last_moved = std::chrono::system_clock::now();

    std::random_device rd;
    rand_eng.seed(rd());

    generator = std::uniform_int_distribution<int>(1, height > width ? height : width);
    gen_food();
}

Board::~Board()
{
    endwin(); // Restore terminal settings
}

void Board::update_screen()
{
    wmove(win, 0, 0);
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (j == 0 || i == 0 || j == width - 1 || i == height - 1)
            {
                waddch(win, ACS_BLOCK | A_BOLD | COLOR_PAIR(WALLCOLOR));
            }
            else if (snake.ishead(i, j))
            {
                waddch(win, '0' | COLOR_PAIR(SNAKECOLOR));
            }
            else if (snake.occupies(i, j))
            {
                waddch(win, 'O' | COLOR_PAIR(SNAKECOLOR));
            }
            else if (food == (Coord{i, j}))
            {
                waddch(win, 'A' | COLOR_PAIR(FOODCOLOR));
            }
            else
            {
                waddch(win, ' ');
            }
        }
    }
    wmove(win, 0, 0);
    waddstr(win, " Score ");
    waddstr(win, std::to_string(score).c_str());
    wrefresh(win);
}

void Board::run()
{
    int ch = 0;
    while (ch != 'q')
    {
        if ((ch = getch()) != ERR)
        {
            switch (ch)
            {
            case KEY_UP:
                snake.redirect(North);
                break;
            case KEY_DOWN:
                snake.redirect(South);
                break;
            case KEY_RIGHT:
                snake.redirect(East);
                break;
            case KEY_LEFT:
                snake.redirect(West);
                break;
            case 'p':
                paused = !paused;
                if (paused)
                {
                    wmove(win, (height / 2), (width / 2) - 5);
                    waddstr(win, "PAUSED");
                    wrefresh(win);
                }
                break;
            case 'c':
                if (paused)
                {
                    wmove(win, height - 1, 0);
                    waddstr(win, "Cheat code: ");
                    wrefresh(win);
                    nocbreak();
                    echo();
                    curs_set(1);
                    nodelay(win, false);
                    char cstr[20];
                    getstr(cstr);
                    std::string str = cstr;
                    auto twentymills = std::chrono::milliseconds(20);
                    if (str == "faster")
                    {
                        if (speed_time > twentymills)
                        {
                            speed_time -= twentymills;
                        }
                    }
                    else if (str == "slower")
                    {
                        speed_time += twentymills;
                    }
                    else if (str == "speeddemon")
                    {
                        speed_time = std::chrono::milliseconds(5);
                    }
                    else if (str == "slowpoke")
                    {
                        speed_time = std::chrono::milliseconds(500);
                    }
                    else if (str == "greedy")
                    {
                        multiplier++;
                    }
                    else if (str == "greedier")
                    {
                        multiplier += 10;
                    }
                    else if (str == "immortal")
                    {
                        immortal = !immortal;
                    }
                    else if (str == "saint")
                    {
                        immortal = false;
                        speed_time = std::chrono::milliseconds(100);
                        multiplier = 1;
                    }
                    curs_set(0);
                    nodelay(win, true);
                    cbreak();
                    noecho();
                    paused = false;
                }
                break;
            case 'r':
                if (dead)
                {
                    snake = Snake();
                    snake.begin(height / 2, width / 2);
                    dead = false;
                    score = 0;
                    gen_food();
                }
            }
        }
        if (paused || dead)
        {
            continue;
        }
        auto now = std::chrono::system_clock::now();
        if (now - last_moved >= speed_time)
        {
            auto next_move = snake.next_move();
            last_moved = now;
            if (next_move.x == 0 || next_move.y == 0 || next_move.x == height - 1 || next_move.y == width - 1 || snake.occupies(next_move))
            {
                if (immortal)
                    continue;
                dead = true;
                wmove(win, (height / 2), (width / 2) - 5);
                waddstr(win, "GAME OVER");
                wrefresh(win);
                continue;
            }
            bool eat = next_move == food;
            snake.move();
            if (eat)
            {
                snake.eat(multiplier);
                score += multiplier;
                gen_food();
            }
        }
        now = std::chrono::system_clock::now();

        if (now - food_created >= food_time)
        {
            gen_food();
        }

        update_screen();
    }
}

void Board::gen_food()
{
    Coord xy;
    do
    {
        xy = {generator(rand_eng), generator(rand_eng)};
    } while (snake.occupies(xy.x, xy.y) || xy.x >= height || xy.y >= width);

    Coord snakehead = snake.head();
    int xx = abs(snakehead.x - xy.x) + abs(snakehead.y - xy.y);
    xx += xx / 2;
    std::uniform_int_distribution<int> gen(xx, xx * 2);
    food_time = speed_time * gen(rand_eng);
    food_created = std::chrono::system_clock::now();
    food = xy;
}
