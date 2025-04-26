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
};

struct Maze
{
    int current_room_row = 0;
    int current_room_col = 0;
    std::vector<Room> rooms;
    int base_cnt = 0;
};

struct MazeData;
struct RoomData;
struct Assets;

void create_room(Room & ro, Maze & mo, const RoomData & room, const Assets & assets);
void create_maze(Maze & mo, const MazeData & maze, const Assets & assets);
Room & get_current_room(Maze & mo);
void update_item(Animation & item, int gFrame);

#endif
