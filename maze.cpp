#include <iostream>
#include <fstream>

#include "maze.h"
#include "maze_data.h"
#include "assets.h"
#include "zx_colors.h"

void create_walls(Room & ro, const RoomData & roomData, const Assets & assets)
{
    for (size_t i=0; i<roomData.walls.size(); ++i)
    {
        const sf::Texture & tex = assets.walls[roomData.walls[i].texid];

        Animation anim;
        create_animation(anim, roomData.walls[i].texid, tex, tex.getSize().x, tex.getSize().y);
        anim.sprite.value().setPosition(
            {static_cast<float>(roomData.walls[i].x) + anim.half_size.x, 
             static_cast<float>(roomData.walls[i].y) + anim.half_size.y + 100});

        ro.walls.push_back(anim);
    }
}

void create_room(Room & ro, Maze & mo, const RoomData & roomData, const MazeData & mazeData, const Assets & assets)
{
    // walls
    create_walls(ro, roomData, assets);

    // items
    for (size_t i=0; i<roomData.items.size(); ++i)
    {
        const sf::Texture & tex = assets.items[roomData.items[i].id];

        Animation anim;
        create_animation(anim, roomData.items[i].id, tex, tex.getSize().x, tex.getSize().y);
        anim.sprite.value().setPosition(
            {static_cast<float>(roomData.items[i].x) + anim.half_size.x, 
             static_cast<float>(roomData.items[i].y) + anim.half_size.y + 100 + 6});
        anim.color_index = 0;

        ro.items.push_back(anim);
    }

    // enemies
    for (size_t i=0; i<roomData.enemies.size(); ++i)
    {
        Enemy eo;
        create_enemy(eo, roomData.enemies[i], mazeData, assets);
        if (eo.anim.id == 0)
        {
            mo.base_cnt++;
        }
        if (eo.anim.id == 20 && eo.carried_enemy.has_value())
        {
            if (eo.carried_enemy.value().id == 0)
            {
                mo.base_cnt++;
            }
        }
        ro.enemies.push_back(eo);
    }
}

void create_maze(Maze & mo, const MazeData & mazeData, const Assets & assets)
{
    for (const auto & roomData : mazeData.rooms)
    {
        Room ro;
        create_room(ro, mo, roomData, mazeData, assets);
        mo.rooms.push_back(ro);
    }
    // std::cout << "Bases: " << mo.base_cnt << "\n";
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

void load_maze(Maze & mo, const MazeData & mazeData, const Assets & assets, int slot)
{
    // std::cout << "Load maze: " << slot << "\n";

    std::ifstream input;
    input.open(std::format("./save{:03d}m", slot), std::ios::binary);
    if (input.is_open())
    {
        // base count loaded to separate value, it will be used after all rooms are created
        uint16_t baseCnt;

        input.read(reinterpret_cast <char*>(&baseCnt), sizeof(uint16_t));
        input.read(reinterpret_cast <char*>(&mo.current_room_row), sizeof(uint16_t));
        input.read(reinterpret_cast <char*>(&mo.current_room_col), sizeof(uint16_t));
        input.read(reinterpret_cast <char*>(&mo.score), sizeof(uint16_t));

        std::vector<bool> visited;
        bool tmp;
        for (int i=0; i<ROWS*COLS; ++i)
        {
            input.read(reinterpret_cast <char*>(&tmp), sizeof(bool));
            visited.push_back(tmp);
        }

        mo.rooms.clear();
        for (int r=0; r<ROWS; ++r)
        {
            for (int c=0; c<COLS; ++c)
            {
                Room ro;

                int idx = r * COLS + c;
                if (visited[idx])
                {
                    ro.visited = true;

                    create_walls(ro, mazeData.rooms[idx], assets);

                    size_t ecnt;
                    input.read(reinterpret_cast <char*>(&ecnt), sizeof(size_t));

                    for (size_t i = 0; i < ecnt; ++i)
                    {
                        uint16_t id, subid;
                        input.read(reinterpret_cast <char*>(&id), sizeof(uint16_t));
                        input.read(reinterpret_cast <char*>(&subid), sizeof(uint16_t));
                        
                        Enemy eo;
                        create_enemy_anim(eo.anim, id, subid, assets);

                        float x, y;
                        input.read(reinterpret_cast <char*>(&x), sizeof(float));
                        input.read(reinterpret_cast <char*>(&y), sizeof(float));

                        eo.anim.sprite.value().setPosition({x, y});

                        if ((id == 1 || id == 8) && subid == 0)
                        {
                            eo.anim.sprite.value().setScale({-1.0f, 1.0f});
                        }

                        input.read(reinterpret_cast <char*>(&eo.velocity.x), sizeof(float));
                        input.read(reinterpret_cast <char*>(&eo.velocity.y), sizeof(float));
                        input.read(reinterpret_cast <char*>(&eo.health), sizeof(uint16_t));
                        input.read(reinterpret_cast <char*>(&eo.anim.color_index), sizeof(uint16_t));

                        eo.anim.sprite.value().setColor(zx_colors[eo.anim.color_index]);

                        eo.shooting_delay = mazeData.enemy_specs[id].shooting_delay;
                        eo.shooting_speed = mazeData.enemy_specs[id].shooting_speed;
                        eo.shooting_counter = 0;

                        bool ce;
                        input.read(reinterpret_cast <char*>(&ce), sizeof(bool));
                        if (eo.anim.id == 20 && ce)
                        {
                            input.read(reinterpret_cast <char*>(&id), sizeof(uint16_t));
                            input.read(reinterpret_cast <char*>(&subid), sizeof(uint16_t));

                            eo.carried_enemy = std::make_optional<Animation>();
                            create_enemy_anim(eo.carried_enemy.value(), id, subid, assets);

                            if ((id == 1 || id == 8) && subid == 0)
                            {
                                eo.carried_enemy.value().sprite.value().setScale({-1.0f, 1.0f});
                            }

                            input.read(reinterpret_cast <char*>(&eo.carried_enemy_health), sizeof(uint16_t));
                            input.read(reinterpret_cast <char*>(&eo.carried_enemy.value().color_index), sizeof(uint16_t));

                            eo.carried_enemy.value().sprite.value().setColor(zx_colors[eo.carried_enemy.value().color_index]);

                            eo.shooting_delay = mazeData.enemy_specs[id].shooting_delay;
                            eo.shooting_speed = mazeData.enemy_specs[id].shooting_speed;
                            eo.shooting_counter = 0;
                        }

                        bool ci;
                        input.read(reinterpret_cast <char*>(&ci), sizeof(bool));
                        if (eo.anim.id == 20 && ci)
                        {
                            input.read(reinterpret_cast <char*>(&id), sizeof(uint16_t));
                            input.read(reinterpret_cast <char*>(&subid), sizeof(uint16_t));

                            eo.carried_item = std::make_optional<Animation>();
                            const sf::Texture & tex = assets.items[id];
                            create_animation(eo.carried_item.value(), id, tex, tex.getSize().x, tex.getSize().y);
                        }

                        ro.enemies.push_back(eo);
                    }

                    size_t icnt;
                    input.read(reinterpret_cast <char*>(&icnt), sizeof(size_t));
                    for (size_t i = 0; i < icnt; ++i)
                    {
                        uint16_t id, subid;
                        float x, y;

                        input.read(reinterpret_cast <char*>(&id), sizeof(uint16_t));
                        input.read(reinterpret_cast <char*>(&subid), sizeof(uint16_t));
                        input.read(reinterpret_cast <char*>(&x), sizeof(float));
                        input.read(reinterpret_cast <char*>(&y), sizeof(float));

                        Animation anim;
                        const sf::Texture & tex = assets.items[id];
                        create_animation(anim, id, tex, tex.getSize().x, tex.getSize().y);
                        anim.sprite.value().setPosition({x, y});
                        anim.color_index = 0;

                        ro.items.push_back(anim);
                    }
                }
                else
                {
                    create_room(ro, mo, mazeData.rooms[idx], mazeData, assets);
                    ro.visited = false;
                }

                mo.rooms.push_back(ro);
            }
        }

        mo.base_cnt = baseCnt;
        mo.created = true;

        input.close();
    }
}

void save_maze(Maze & mo, int slot)
{
    // std::cout << "Save maze: " << slot << "\n";

    std::ofstream output;
    output.open(std::format("./save{:03d}m", slot), std::ios::binary);
    if (output.is_open())
    {
        output.write(reinterpret_cast <char*>(&mo.base_cnt), sizeof(uint16_t));
        output.write(reinterpret_cast <char*>(&mo.current_room_row), sizeof(uint16_t));
        output.write(reinterpret_cast <char*>(&mo.current_room_col), sizeof(uint16_t));
        output.write(reinterpret_cast <char*>(&mo.score), sizeof(uint16_t));

        for (auto & room : mo.rooms)
        {
            output.write(reinterpret_cast <char*>(&room.visited), sizeof(bool));
        }

        for (auto & room : mo.rooms)
        {
            if (room.visited)
            {
                size_t ecnt = room.enemies.size();
                output.write(reinterpret_cast <char*>(&ecnt), sizeof(size_t));

                for (auto & enemy : room.enemies)
                {
                    float x = enemy.anim.sprite.value().getPosition().x;
                    float y = enemy.anim.sprite.value().getPosition().y;

                    output.write(reinterpret_cast <char*>(&enemy.anim.id), sizeof(uint16_t));
                    output.write(reinterpret_cast <char*>(&enemy.anim.subid), sizeof(uint16_t));
                    output.write(reinterpret_cast <char*>(&x), sizeof(float));
                    output.write(reinterpret_cast <char*>(&y), sizeof(float));
                    output.write(reinterpret_cast <char*>(&enemy.velocity.x), sizeof(float));
                    output.write(reinterpret_cast <char*>(&enemy.velocity.y), sizeof(float));
                    output.write(reinterpret_cast <char*>(&enemy.health), sizeof(uint16_t));
                    output.write(reinterpret_cast <char*>(&enemy.anim.color_index), sizeof(uint16_t));

                    bool ce = enemy.carried_enemy.has_value();
                    output.write(reinterpret_cast <char*>(&ce), sizeof(bool));
                    if (enemy.anim.id == 20 && ce)
                    {
                        output.write(reinterpret_cast <char*>(&enemy.carried_enemy.value().id), sizeof(uint16_t));
                        output.write(reinterpret_cast <char*>(&enemy.carried_enemy.value().subid), sizeof(uint16_t));
                        output.write(reinterpret_cast <char*>(&enemy.carried_enemy_health), sizeof(uint16_t));
                        output.write(reinterpret_cast <char*>(&enemy.carried_enemy.value().color_index), sizeof(uint16_t));
                    }

                    bool ci = enemy.carried_item.has_value();
                    output.write(reinterpret_cast <char*>(&ci), sizeof(bool));
                    if (enemy.anim.id == 20 && ci)
                    {
                        output.write(reinterpret_cast <char*>(&enemy.carried_item.value().id), sizeof(uint16_t));
                        output.write(reinterpret_cast <char*>(&enemy.carried_item.value().subid), sizeof(uint16_t));
                    }
                }

                size_t icnt = room.items.size();
                output.write(reinterpret_cast <char*>(&icnt), sizeof(size_t));

                for (auto & item : room.items)
                {
                    float x = item.sprite.value().getPosition().x;
                    float y = item.sprite.value().getPosition().y;

                    output.write(reinterpret_cast <char*>(&item.id), sizeof(uint16_t));
                    output.write(reinterpret_cast <char*>(&item.subid), sizeof(uint16_t));
                    output.write(reinterpret_cast <char*>(&x), sizeof(float));
                    output.write(reinterpret_cast <char*>(&y), sizeof(float));
                }
            }
        }

        output.close();
    }
}
