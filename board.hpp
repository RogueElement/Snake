#include <chrono>
#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <thread>
#include <curses.h>

#include "snake.hpp"

#define BOARDSIZE 50

#define WALLCOLOR 1
#define FOODCOLOR 2
#define SNAKECOLOR 3

class Board
{
public:
    Board(int speed);
    void run();
    ~Board();

private:
    int height, width, score;
    bool paused, dead;

    coord food;
    Snake snake;
    WINDOW *win;

    std::vector<std::vector<chtype>> board;
    std::chrono::system_clock::time_point last_moved, food_created;
    std::chrono::milliseconds speed_time, food_time;
    std::uniform_int_distribution<int> generator;
    std::mt19937 rand_eng;

    void update_screen();
    void gen_food();
    void game_over();
};

Board::Board(int speed)
    : speed_time(std::chrono::milliseconds(speed)), paused(false), dead(false), food({0, 0}), score(0)
{
    initscr();
    getmaxyx(stdscr, height, width);
    // int min = height < width ? height : width;
    // height = min;
    // width = min;
    win = stdscr;
    cbreak();
    noecho();
    nodelay(win, true);
    keypad(win, true);
    start_color();
    std::cerr << "Logging\n";

    snake.begin(height / 2, width / 2);

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
    endwin();
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
            else if (food == (coord{i, j}))
            {
                waddch(win, 'A' | COLOR_PAIR(FOODCOLOR));
            }
            else
            {
                waddch(win, ' ');
            }
        }
        //wmove(win, i, 0);
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
                break;
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
                game_over();
                continue;
            }
            bool eat = next_move == food;
            snake.move();
            if (eat)
            {
                snake.eat();
                score++;
                gen_food();
            }
        }
        now = std::chrono::system_clock::now();

        if (now - food_created >= food_time)
        {
            gen_food();
        }

        update_screen();
        //std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void Board::gen_food()
{
    coord xy;
    do
    {
        xy = {generator(rand_eng), generator(rand_eng)};
    } while (snake.occupies(xy.x, xy.y) || xy.x >= height || xy.y >= width);
    
    coord snakehead = snake.head();
    int xx = abs(snakehead.x - xy.x) + abs(snakehead.y - xy.y);
    xx += xx / 2;
    std::uniform_int_distribution<int> gen(xx, xx * 2);
    food_time = speed_time * gen(rand_eng);
    food_created = std::chrono::system_clock::now();
    food = xy;
}

void Board::game_over()
{
    dead = true;
    wmove(win, (height / 2), (width / 2) - 5);
    waddstr(win, "GAME OVER");
    wrefresh(win);
}