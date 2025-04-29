#include <iostream>
#include <fstream>
#include <sstream>
#include <format>

#include "maze_data.h"

void load_wall_data(RoomData & room, const int & row, const int & col)
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
            WallData w;
            input >> sep >> w.texid >> w.x >> w.y;
            room.walls.push_back(w);
        }
        input.close();
    }
}

void load_item_data(RoomData & room, const int & row, const int & col)
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
            ItemData it;
            input >> sep >> it.x >> it.y >> it.id;
            room.items.push_back(it);
        }
        input.close();
    }
}

void load_enemy_data(RoomData & room, const int & row, const int & col)
{
    auto fileName = std::format("./data/enemies/enemy{}{}.txt", row, col);
    std::fstream input;
    input.open(fileName, std::ios::in);
    if (input.is_open())
    {
        int cnt, ctype, dummy;
        char sep;
        input >> cnt;
        for (int8_t i=0; i<cnt; ++i)
        {
            EnemyData en;
            input >> sep >> en.x >> en.y >> en.id >> en.subid;
            if (en.id == 20)
            {
                input >> sep >> ctype;
                if (ctype == 0)
                {
                    en.carried_enemy = std::make_optional<CarriedEnemyData>();
                    auto & cen = en.carried_enemy.value();
                    input >> cen.x >> cen.y >> cen.id >> cen.subid;
                }
                else
                {
                    en.carried_item = std::make_optional<ItemData>();
                    auto & cit = en.carried_item.value();
                    input >> cit.x >> cit.y >> cit.id >> dummy;
                }
            }
            room.enemies.push_back(en);
        }
        input.close();
    }
}

void load_room_data(RoomData & room, const int & row, const int & col)
{
    room.row = row;
    room.col = col;

    load_wall_data(room, row, col);
    load_item_data(room, row, col);
    load_enemy_data(room, row, col);
}

void load_enemy_specs(MazeData & maze)
{
    std::fstream input;
    input.open("./data/enemies/general.txt", std::ios::in);
    if (input.is_open())
    {
        std::string line;
        int line_idx = 0;
        while (std::getline(input, line))
        {
            if (line_idx == 0)
            {
                line_idx++;
                continue;
            }

            std::stringstream ss(line);
            EnemySpec spc;
            int id;
            char del;
            ss >> id >> del >> spc.health >> del >> spc.shooting_delay >> del >> spc.shooting_speed;
            maze.enemy_specs.push_back(spc);

            line_idx++;
        }

        input.close();
    }
}

void load_maze_data(MazeData & maze)
{
    for (int r=0; r<ROWS; ++r)
    {
        for (int c=0; c<COLS; ++c)
        {
            RoomData room;
            load_room_data(room, r, c);
            maze.rooms.push_back(room);
        }
    }

    load_enemy_specs(maze);
}
