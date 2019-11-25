#include <deque>
#include <algorithm>

struct Coord
{
    int x, y;

    bool operator==(const Coord a)
    {
        return a.x == x && a.y == y;
    }
};

enum Direction
{
    North,
    West,
    East,
    South
};

class Snake
{
public:
    // Creates a new snake with no body
    // facing North
    Snake();
    // Adds a head to the Snake at Coord(x, y)
    // and a next at Coord(x, y+1)
    void begin(int x, int y);
    // Checks is any of the Snake parts is at
    // Coord(x,y)
    bool occupies(int x, int y);
    bool occupies(Coord xy);
    bool ishead(int x, int y);
    Coord next_move(), head();
    void move(), redirect(Direction direct), eat();

private:
    int growth;
    Direction dir;
    std::deque<Coord> coords;
};

Snake::Snake()
    : dir(North), growth(0)
{
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

bool Snake::occupies(Coord xy)
{
    return std::find(coords.begin(), coords.end(), xy) != coords.end();
}
bool Snake::occupies(int x, int y)
{
    return std::find(coords.begin(), coords.end(), Coord{x, y}) != coords.end();
}

Coord Snake::next_move()
{
    Coord xy = coords.front();
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
    Coord xy = next_move();
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

void Snake::redirect(Direction direct)
{   
    // Don't redirect to the opposite direction
    // so the snake doesn't try to enter it's neck
    if (direct + dir != 3)
        dir = direct;
}

Coord Snake::head()
{
    return coords.front();
}
