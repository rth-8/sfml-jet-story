#ifndef OBJECTS_H
#define OBJECTS_H

namespace js {
namespace GameObjects {

struct Room
{
    std::vector<Animation> walls;
    std::vector<Animation> items;
    std::vector<Enemy> enemies;
};

struct Maze
{
    int current_room_row = 0;
    int current_room_col = 0;
    std::vector<Room> rooms;
};

void create_room(Room & ro, const js::Data::Room & room, const Assets & assets)
{
    for (int i=0; i<room.walls.size(); ++i)
    {
        const sf::Texture & tex = assets.walls[room.walls[i].texid];

        Animation anim;
        create_animation(anim, room.walls[i].texid, tex, tex.getSize().x, tex.getSize().y);
        anim.sprite.value().setPosition(
            {static_cast<float>(room.walls[i].x) + anim.half_size.x, 
             static_cast<float>(room.walls[i].y) + anim.half_size.y + 100});

        ro.walls.push_back(anim);
    }

    for (int i=0; i<room.items.size(); ++i)
    {
        const sf::Texture & tex = assets.items[room.items[i].id];

        Animation anim;
        create_animation(anim, room.items[i].id, tex, tex.getSize().x, tex.getSize().y);
        anim.sprite.value().setPosition(
            {static_cast<float>(room.items[i].x) + anim.half_size.x, 
             static_cast<float>(room.items[i].y) + anim.half_size.y + 100 + 6});
        anim.color_index = 0;

        ro.items.push_back(anim);
    }

    for (int i=0; i<room.enemies.size(); ++i)
    {
        Enemy eo;
        create_enemy(eo, room.enemies[i], assets);
        ro.enemies.push_back(eo);
    }
}

void create_maze(Maze & mo, const js::Data::Maze & maze, const Assets & assets)
{
    for (const auto & room : maze.rooms)
    {
        Room ro;
        create_room(ro, room, assets);
        mo.rooms.push_back(ro);
    }
}

Room & get_current_room(Maze & mo)
{
    return mo.rooms[mo.current_room_row * COLS + mo.current_room_col];
}

void update_item(Animation & item, int gFrame)
{
    if (gFrame % 6 == 0)
    {
        item.color_index = (item.color_index + 1) % item_colors_loop.size();
        item.sprite.value().setColor(zx_colors[item_colors_loop[item.color_index]]);
    }
}

} // namespace GameObjects
} // namespace js

#endif
