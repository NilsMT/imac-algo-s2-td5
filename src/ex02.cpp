#include <map>
#include <iostream>

struct Position
{
    int x = 0;
    int y = 0;
};

enum Direction { Haut, Droite, Bas, Gauche };

std::map<Direction, Position> d_to_p = {
    {Haut, {0,-1}},
    {Droite, {1,0}},
    {Bas, {0,1}},
    {Gauche, {-1,0}}
};

///////////////////////////////////////////////////
// Operators
///////////////////////////////////////////////////

//https://dsmte.github.io/Learn--cpp_programming/Lessons/S2/Operators

bool operator==(Position const& a, Position const& b) {
    return a.x == b.x && a.y == b.y;
}

std::ostream& operator<<(std::ostream& os, Position const& p) {
    os << "(" << p.x << "," << p.y << ")";
    return os;
}

Position operator+=(Position& a, Position const& b) {
    a.x += b.x;
    a.y += b.y;
    return a;
}

Position& operator+(Position& p, Direction const& d) {
    p += d_to_p[d];
    return p;
}

Position& operator+=(Position& p, Direction const& d) {
    p += d_to_p[d];
    return p;
}

///////////////////////////////////////////////////

void turn_right(Direction& d) {
    switch (d)
    {
    case Direction::Haut:
        d = Direction::Droite;
        break;
    case Direction::Droite:
        d = Direction::Bas;
        break;
    case Direction::Bas:
        d = Direction::Gauche;
        break;
    case Direction::Gauche:
        d = Direction::Haut;
        break;
    default:
        break;
    }
}

int main() {

    return 0;
}