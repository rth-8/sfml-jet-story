#ifndef MAZE_H
#define MAZE_H

#include <vector>
#include <fstream>
#include <iostream>

#define ROWS 8
#define COLS 16

namespace js {
namespace Data {

struct Wall
{
    int16_t x, y;
    int texid;
};

struct Item
{
    int16_t x, y;
    int id;
};

struct Room
{
    int row, col;
    std::vector<Wall> walls;
    std::vector<Item> items;
};

struct Maze
{
    std::vector<Room> rooms;
};

void load_walls(Room & room, const int & row, const int & col)
{
    auto fileName = std::format("./data/rooms/room{}{}.txt", row, col);
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

void load_items(Room & room, const int & row, const int & col)
{
    auto fileName = std::format("./data/items/item{}{}.txt", row, col);
    std::fstream input;
    input.open(fileName, std::ios::in);
    if (input.is_open())
    {
        int cnt;
        char sep;
        input >> cnt;
        for (int8_t i=0; i<cnt; ++i)
        {
            Item it;
            input >> sep >> it.x >> it.y >> it.id;
            room.items.push_back(it);
        }
        input.close();
    }
}

void load_room(Room & room, const int & row, const int & col)
{
    room.row = row;
    room.col = col;

    load_walls(room, row, col);
    load_items(room, row, col);
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
