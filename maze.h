#ifndef MAZE_H
#define MAZE_H

#include <vector>
#include <fstream>
// #include <iostream>

#define ROWS 8
#define COLS 16

namespace js {
namespace Data {

struct Wall
{
    int16_t x, y;
    int texid;
};

struct Room
{
    int row, col;
    std::vector<Wall> walls;
};

struct Maze
{
    std::vector<Room> rooms;
};

void load_room(Room & room, const int & row, const int & col)
{
    room.row = row;
    room.col = col;
    
    auto fileName = std::format("./data/rooms/room{}{}.txt", row, col);
    // std::cout << "Load room: " << fileName << '\n';
    
    std::fstream input;
    input.open(fileName, std::ios::in);
    if (input.is_open())
    {
        int cnt;
        char sep;
        input >> cnt;
        for (int8_t i=0; i<cnt; ++i)
        {
            Wall w;
            input >> sep >> w.texid >> w.x >> w.y;
            room.walls.push_back(w);
        }
        input.close();
    }
}

void load_maze(Maze & maze)
{
    for (int r=0; r<ROWS; ++r)
    {
        for (int c=0; c<COLS; ++c)
        {
            Room room;
            load_room(room, r, c);
            maze.rooms.push_back(room);
        }
    }
}

} // namespace Data
} // namespace js

#endif
