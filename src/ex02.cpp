#include <map>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <set>
#include <unordered_set>

///////////////////////////////////////////////////
// Tiles characters
///////////////////////////////////////////////////

enum Tile { 
    Empty = '.', 
    Obstacle = '#', 
    Visited = 'X',
    Guard_up = '^',
    Guard_down = 'V',
    Guard_left = '<',
    Guard_right = '>'
};

///////////////////////////////////////////////////
// Direction
///////////////////////////////////////////////////

enum Direction { Haut, Droite, Bas, Gauche };

Direction turn_right(Direction& d) {
    switch (d)
    {
    case Direction::Haut:
        return Direction::Droite;
        break;
    case Direction::Droite:
        return Direction::Bas;
        break;
    case Direction::Bas:
        return Direction::Gauche;
        break;
    case Direction::Gauche:
        return Direction::Haut;
        break;
    default:
        throw std::runtime_error("Invalid direction");
    }
}

///////////////////////////////////////////////////
// Position
///////////////////////////////////////////////////

struct Position
{
    int x = 0;
    int y = 0;

    bool operator==(Position const& b) const {
        return x == b.x && y == b.y;
    }

    //Jules told me this could be used so I can have the operator in the struct
    friend std::ostream& operator<<(std::ostream& os, Position const& p) {
        os << "(" << p.x << "," << p.y << ")";
        return os;
    }

    Position operator+=(Position const& b) {
        x += b.x;
        y += b.y;
        return *this;
    }

    bool operator<(Position const& b) const {
        return x < b.x || (x == b.x && y < b.y);
    }
};

namespace std {
    template <>
    struct hash<Position> {
        std::size_t operator()(const Position& pos) const {
            //used hashing technique: https://fr.wikipedia.org/wiki/Fonction_de_couplage#Fonction_de_couplage_de_Cantor
            return ((pos.x + pos.y) * (pos.x + pos.y + 1)) / 2 + pos.y;
        }
    };
}

///////////////////////////////////////////////////
// Position & Direction Operators and Binds
///////////////////////////////////////////////////

std::map<Direction,std::pair<Position,char>> d_to_p = {
    {Haut, {{0,-1}, Tile::Guard_up}},
    {Droite, {{1,0}, Tile::Guard_right}},
    {Bas, {{0,1}, Tile::Guard_down}},
    {Gauche, {{-1,0}, Tile::Guard_left}}
};

Position operator+(Position p, Direction const& d) {
    p += d_to_p[d].first;
    return p;
}

Position& operator+=(Position& p, Direction const& d) {
    p += d_to_p[d].first;
    return p;
}

///////////////////////////////////////////////////
// PatrolMap
///////////////////////////////////////////////////

struct PatrolMap {
    std::vector<std::vector<char>> tiles{};
    std::set<Position> obstacles{};
    std::pair<Position, Direction> guard{};

    //Jules told me this could be used so I can have the operator in the struct
    friend std::ostream& operator<<(std::ostream& os, PatrolMap const& pm) {
        for (const auto& row : pm.tiles) {
            for (char c : row) {
                os << c;
            }
            os << '\n';
        }

        os << "Guard position: " << pm.guard.first << " Facing: " << d_to_p[pm.guard.second].second << "\n";

        return os;
    }

    bool is_position_valid(Position const& pos) const {
        return pos.y >= 0 && pos.y < tiles.size() && pos.x >= 0 && pos.x < tiles[pos.y].size();
    }
};

//Utils to know what is defined as a "valid character" for the map
bool is_char_valid(char c) {
    return c == Tile::Empty || c == Tile::Obstacle || c == Tile::Visited || c == Tile::Guard_up || c == Tile::Guard_down || c == Tile::Guard_left || c == Tile::Guard_right;
}

PatrolMap parse_input(std::istream& input_stream) {
    int x = 0;
    int y = 0;

    PatrolMap pm;

    for (std::string line{}; std::getline(input_stream, line, '\n') and line != "";) {
        x = 0;
        std::vector<char> vec = std::vector<char>(line.begin(), line.end());
        pm.tiles.push_back(vec);

        for(char c : line) {
            if (!is_char_valid(c)) {
                throw std::runtime_error(
                    "Invalid character in input stream: '" + std::string(1, c) + "' at line " + std::to_string(y+1) + ", column " + std::to_string(x+1)
                );
            } else if (c == Tile::Guard_up || c == Tile::Guard_down || c == Tile::Guard_left || c == Tile::Guard_right) {
                Direction d;
                switch (c)
                {
                case Tile::Guard_up:
                    d = Direction::Haut;
                    break;
                case Tile::Guard_down:
                    d = Direction::Bas;
                    break;
                case Tile::Guard_left:
                    d = Direction::Gauche;
                    break;
                case Tile::Guard_right:
                    d = Direction::Droite;
                    break;
                default:
                    break;
                }
                pm.guard = std::make_pair(Position{x,y}, d);
            } else if (c == Tile::Obstacle) {
                pm.obstacles.insert(Position{x,y});
            }

            pm.tiles[y][x] = c;
            x++;
        }
        y++;
    }

    return pm;
}


///////////////////////////////////////////////////
// WalkResult
///////////////////////////////////////////////////

struct WalkResult {
    Position final_position;
    size_t steps_taken;
    std::unordered_set<Position> visited_positions;

    friend std::ostream& operator<<(std::ostream& os, WalkResult const& wr) {
        os << "Final position: " << wr.final_position << "\n";
        os << "Steps taken: " << wr.steps_taken << "\n";
        return os;
    }
};

///////////////////////////////////////////////////
// Functions
///////////////////////////////////////////////////

WalkResult move_guard_across_map(PatrolMap& pm) {
    WalkResult result{};

    std::cout << "Starting position: " << pm.guard.first << "\n";

    while (result.steps_taken < 100000) {
        Position next_pos = pm.guard.first + pm.guard.second;

        if (pm.is_position_valid(next_pos)) {
            if (pm.obstacles.find(next_pos) != pm.obstacles.end()) {
                //it will be an obstacle, turn right
                pm.guard.second = turn_right(pm.guard.second);
                pm.tiles[pm.guard.first.y][pm.guard.first.x] = d_to_p[pm.guard.second].second;
            } else {
                //it will be an empty tile, move forward
                pm.tiles[pm.guard.first.y][pm.guard.first.x] = Tile::Visited;
                pm.guard.first = next_pos;
                pm.tiles[pm.guard.first.y][pm.guard.first.x] = d_to_p[pm.guard.second].second;
                
                // Only count as a step if it's a new position
                if (result.visited_positions.find(pm.guard.first) == result.visited_positions.end()) {
                    result.steps_taken++;
                }
                result.visited_positions.insert(pm.guard.first);
            }
        } else {
            break;
        }
    }

    pm.tiles[pm.guard.first.y][pm.guard.first.x] = Tile::Visited;

    std::cout << "Final map state:\n" << pm << "\n";

    result.final_position = pm.guard.first;
    return result;
}

///////////////////////////////////////////////////
// Main
///////////////////////////////////////////////////

int main() {
    std::ifstream myfile;
    myfile.open("../src/input_guard_patrol.txt");

    PatrolMap pm = parse_input(myfile);

    std::cout << pm;

    auto res = move_guard_across_map(pm);
    std::cout << res << "\n";

    return 0;
}