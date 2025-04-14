#ifndef OBJECTS_H
#define OBJECTS_H

#include <iostream>
#include <vector>
#include <optional>

namespace js {
namespace GameObjects {

struct Animation
{
    std::optional<sf::Sprite> sprite;
    sf::Vector2f size;
    sf::Vector2f half_size;
    int frame_count;
    int speed;
    int frame;
};

void create_animation(Animation & anim, const sf::Texture & tex, float fw, float fh, int fc = 1, int spd = 0)
{
    anim.size.x = fw;
    anim.size.y = fh;
    anim.half_size.x = fw * 0.5f;
    anim.half_size.y = fh * 0.5f;
    anim.frame_count = fc;
    anim.speed = spd;
    anim.frame = 0;
    anim.sprite = std::make_optional<sf::Sprite>(tex);
    anim.sprite.value().setTextureRect(
        sf::IntRect({anim.frame * static_cast<int>(anim.size.x), 0},
                    {static_cast<int>(anim.size.x), static_cast<int>(anim.size.y)}));
    anim.sprite.value().setOrigin(anim.half_size);
}

void animation_update(Animation & anim, int gFrame)
{
    if (anim.frame_count > 1 && anim.speed > 0 && gFrame > 0)
    {
        anim.frame = (gFrame / anim.speed) % anim.frame_count;
        anim.sprite.value().setTextureRect(
            sf::IntRect({anim.frame * static_cast<int>(anim.size.x), 0}, 
                        {static_cast<int>(anim.size.x), static_cast<int>(anim.size.y)}));
    }
}

struct Room
{
    std::vector<Animation> walls;
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
        create_animation(anim, tex, tex.getSize().x, tex.getSize().y);
        anim.sprite.value().setPosition(
            {static_cast<float>(room.walls[i].x) + anim.half_size.x, 
             static_cast<float>(room.walls[i].y) + anim.half_size.y + 100});

        ro.walls.push_back(anim);
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

struct Ship
{
    Animation ship_body;
    Animation ship_flame_down_big;
    Animation ship_flame_down_small;
    Animation ship_flame_back;
    sf::Vector2f previous_position;
    sf::Vector2f velocity;
    bool thrust_up;
    bool thrust_horiz;
};

void create_ship(Ship & ship, const sf::Vector2f & pos, const Assets & assets)
{
    create_animation(ship.ship_body, assets.ship, assets.ship.getSize().x, assets.ship.getSize().y);
    ship.ship_body.sprite.value().setPosition(pos);

    create_animation(ship.ship_flame_back, assets.ship_flame_back, 43, 35, 2, 4);
    create_animation(ship.ship_flame_down_big, assets.ship_flame_down_big, 32, 44, 2, 4);
    create_animation(ship.ship_flame_down_small, assets.ship_flame_down_small, 19, 38, 2, 4);

    ship.previous_position = pos;
    ship.velocity = {0.0f, 0.0f};

    ship.thrust_up = false;
    ship.thrust_horiz = false;
}

void update_ship(Ship & ship)
{
    if (ship.ship_body.sprite.value().getScale().x > 0)
    {
        ship.ship_flame_back.sprite.value().setPosition(
            {ship.ship_body.sprite.value().getPosition().x - ship.ship_body.half_size.x - ship.ship_flame_back.half_size.x, 
             ship.ship_body.sprite.value().getPosition().y - 6});

        ship.ship_flame_down_big.sprite.value().setPosition(
            {ship.ship_body.sprite.value().getPosition().x - 19, 
             ship.ship_body.sprite.value().getPosition().y + ship.ship_body.half_size.y + ship.ship_flame_down_big.half_size.y});

        ship.ship_flame_down_small.sprite.value().setPosition(
            {ship.ship_body.sprite.value().getPosition().x + 22, 
             ship.ship_body.sprite.value().getPosition().y + ship.ship_body.half_size.y + ship.ship_flame_down_small.half_size.y});
    }
    else
    {
        ship.ship_flame_back.sprite.value().setPosition(
            {ship.ship_body.sprite.value().getPosition().x + ship.ship_body.half_size.x + ship.ship_flame_back.half_size.x, 
             ship.ship_body.sprite.value().getPosition().y - 6});

        ship.ship_flame_down_big.sprite.value().setPosition(
            {ship.ship_body.sprite.value().getPosition().x + 19, 
             ship.ship_body.sprite.value().getPosition().y + ship.ship_body.half_size.y + ship.ship_flame_down_big.half_size.y});

        ship.ship_flame_down_small.sprite.value().setPosition(
            {ship.ship_body.sprite.value().getPosition().x - 22, 
             ship.ship_body.sprite.value().getPosition().y + ship.ship_body.half_size.y + ship.ship_flame_down_small.half_size.y});
    }
    ship.ship_flame_back.sprite.value().setScale(ship.ship_body.sprite.value().getScale());
}

} // namespace GameObjects
} // namespace js

#endif
