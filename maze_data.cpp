#include <fstream>
#include <format>

#include "maze_data.h"

void load_walls(RoomData & room, const int & row, const int & col)
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

void load_items(RoomData & room, const int & row, const int & col)
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

void load_enemies(RoomData & room, const int & row, const int & col)
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

void load_room(RoomData & room, const int & row, const int & col)
{
    room.row = row;
    room.col = col;

    load_walls(room, row, col);
    load_items(room, row, col);
    load_enemies(room, row, col);
}

void load_maze(MazeData & maze)
{
    for (int r=0; r<ROWS; ++r)
    {
        for (int c=0; c<COLS; ++c)
        {
            RoomData room;
            load_room(room, r, c);
            maze.rooms.push_back(room);
        }
    }
}
