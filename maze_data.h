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

struct EnemySpec
{
    int health;
    float shooting_delay;
    float shooting_speed;
};

struct MazeData
{
    std::vector<RoomData> rooms;
    std::vector<EnemySpec> enemy_specs;
    uint16_t base_cnt;
};

void load_wall_data(RoomData & room, const int & row, const int & col);
void load_item_data(RoomData & room, const int & row, const int & col);
void load_enemy_data(RoomData & room, MazeData & maze, const int & row, const int & col);
void load_room_data(RoomData & room, MazeData & maze, const int & row, const int & col);
void load_enemy_specs(MazeData & maze);
void load_maze_data(MazeData & maze);

#endif
