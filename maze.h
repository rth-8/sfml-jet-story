#ifndef MAZE_H
#define MAZE_H

#include <vector>

#include "animation.h"
#include "enemies.h"

struct Room
{
    std::vector<Animation> walls;
    std::vector<Animation> items;
    std::vector<Enemy> enemies;
    std::vector<Enemy> to_be_added;
    bool visited = false;
};

struct Maze
{
    std::vector<Room> rooms;
    uint16_t current_room_row = 0;
    uint16_t current_room_col = 0;
    uint16_t base_cnt = 0;
    uint16_t score = 0;
    bool created;
};

struct MazeData;
struct RoomData;
struct Assets;

void create_walls(Room & ro, const RoomData & roomData, const Assets & assets);
void create_room(Room & ro, Maze & mo, const RoomData & roomData, const MazeData & mazeData, const Assets & assets);
void create_maze(Maze & mo, const MazeData & mazeData, const Assets & assets);

Room & get_current_room(Maze & mo);
void update_item(Animation & item, int gFrame);

void load_maze(Maze & mo, const MazeData & mazeData, const Assets & assets, int slot);
void save_maze(Maze & mo, int slot);

#endif
