#include <deque>
#include <algorithm>

struct coord
{
    int x, y;

    bool operator==(const coord a)
    {
        return a.x == x && a.y == y;
    }
};

enum direction
{
    North,
    West,
    East,
    South
};

class Snake
{
public:
    Snake();
    void begin(int x, int y);
    bool occupies(int x, int y);
    bool occupies(coord xy);
    bool ishead(int x, int y);
    coord next_move(), head();
    void move(), redirect(direction direct), eat();

private:
    int growth;
    direction dir;
    std::deque<coord> coords;
};

Snake::Snake()
    : dir(North)
{
    growth = 0;
}

void Snake::begin(int x, int y)
{
    coords.push_back({x, y});
    coords.push_back({x, y + 1});
}

bool Snake::ishead(int x, int y)
{
    return coords.front().x == x && coords.front().y == y;
}

bool Snake::occupies(coord xy)
{
    return std::find(coords.begin(), coords.end(), xy) != coords.end();
}
bool Snake::occupies(int x, int y)
{
    return std::find(coords.begin(), coords.end(), coord{x, y}) != coords.end();
}

coord Snake::next_move()
{
    coord xy = coords.front();
    switch (dir)
    {
    case North:
        xy.x--;
        break;
    case East:
        xy.y++;
        break;
    case South:
        xy.x++;
        break;
    case West:
        xy.y--;
        break;
    }
    return xy;
}

void Snake::move()
{
    coord xy = next_move();
    coords.push_front(xy);
    if (growth > 0)
    {
        growth--;
    }
    else
    {
        coords.pop_back();
    }
}

void Snake::eat()
{
    growth++;
}

void Snake::redirect(direction direct)
{
    if (direct + dir != 3)
        dir = direct;
}

coord Snake::head()
{
    return coords.front();
}
