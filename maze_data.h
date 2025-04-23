#ifndef MAZE_DATA_H
#define MAZE_DATA_H

#include <cstdint>
#include <optional>
#include <vector>

#define ROWS 8
#define COLS 16

struct WallData
{
    int16_t x, y;
    int texid;
};

struct ItemData
{
    int16_t x, y;
    int id;
};

struct CarriedEnemyData
{
    int16_t x, y;
    int id, subid;
};

struct EnemyData
{
    int16_t x, y;
    int id, subid;
    std::optional<CarriedEnemyData> carried_enemy;
    std::optional<ItemData> carried_item;
};

struct RoomData
{
    int row, col;
    std::vector<WallData> walls;
    std::vector<ItemData> items;
    std::vector<EnemyData> enemies;
};

struct MazeData
{
    std::vector<RoomData> rooms;
};

void load_walls(RoomData & room, const int & row, const int & col);
void load_items(RoomData & room, const int & row, const int & col);
void load_enemies(RoomData & room, const int & row, const int & col);
void load_room(RoomData & room, const int & row, const int & col);
void load_maze(MazeData & maze);

#endif
