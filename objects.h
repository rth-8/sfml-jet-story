#ifndef OBJECTS_H
#define OBJECTS_H

#include <iostream>
#include <vector>
#include <optional>

namespace js {
namespace GameObjects {

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

enum SpecialType
{
    BALL,
    MISSILE_SIDE,
    MISSILE_DOWN,
    STAR,
};

#define SPECIAL_BALL_LIFESPAN 12
#define SPECIAL_STAR_LIFESPAN 12

struct Ship
{
    Animation ship_body;
    Animation ship_flame_down_big;
    Animation ship_flame_down_small;
    Animation ship_flame_back;
    sf::Vector2f previous_position;
    sf::Vector2f velocity;
    uint8_t shield;
    uint8_t fuel;
    uint8_t cannon_ammo;

    std::optional<Animation> special;
    SpecialType special_type;
    sf::Vector2f special_previous_position;
    sf::Vector2f special_velocity;
    uint8_t special_ammo;
    float special_lifespan;
    
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

    ship.shield = 100;
    ship.fuel = 100;
    ship.cannon_ammo = 100;

    ship.special_type = SpecialType::STAR;
    ship.special_ammo = 40;

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

void create_special(Ship & ship, const Assets & assets)
{
    ship.special = std::make_optional<Animation>();
    
    switch (ship.special_type)
    {
        case BALL:
            create_animation(ship.special.value(), assets.special_ball, assets.special_ball.getSize().x, assets.special_ball.getSize().y);
            ship.special_velocity = {250.0f, 250.0f};
            break;

        case MISSILE_SIDE:
            create_animation(ship.special.value(), assets.special_missile_side, 
                assets.special_missile_side.getSize().x, assets.special_missile_side.getSize().y);
            ship.special.value().sprite.value().setScale(ship.ship_body.sprite.value().getScale());
            ship.special.value().sprite.value().setColor(sf::Color::Cyan);
            if (ship.special.value().sprite.value().getScale().x < 0)
            {
                // to left
                ship.special_velocity = {-100.0f, 200.0f};
            }
            else
            {
                // to right
                ship.special_velocity = {100.0f, 200.0f};
            }
            break;

        case MISSILE_DOWN:
            create_animation(ship.special.value(), assets.special_missile_down, 
                assets.special_missile_down.getSize().x, assets.special_missile_down.getSize().y);
            ship.special.value().sprite.value().setColor(sf::Color::Cyan);
            ship.special_velocity = {ship.velocity.x * 20.0f, 300.0f};
            break;

        case STAR:
            create_animation(ship.special.value(), assets.special_star, assets.special_star.getSize().x, assets.special_star.getSize().y);
            break;
    }

    ship.special.value().sprite.value().setPosition(ship.ship_body.sprite.value().getPosition());
    ship.special_previous_position = ship.special.value().sprite.value().getPosition();
    ship.special_lifespan = 0;
}

void move_special(float dt, Ship & ship)
{
    switch (ship.special_type)
    {
        case BALL:
            ship.special.value().sprite.value().move({ship.special_velocity.x * dt, ship.special_velocity.y * dt});
            ship.special_lifespan += dt;
            if (ship.special_lifespan >= SPECIAL_BALL_LIFESPAN)
            {
                ship.special.reset();
            }
            break;

        case MISSILE_SIDE:
        {
            sf::Vector2f force{0.0f, 0.0f};
            float accel = 0.0f;
            if (ship.special_velocity.y < -10.0f || ship.special_velocity.y > 10.0f)
            {
                force += {0.0f, -170.0f};
            }
            if (ship.special.value().sprite.value().getScale().x < 0)
            {
                // to left
                force += {-100.0f, 0.0f};
                accel = -150.0f;
            }
            else
            {
                // to right
                force += {100.0f, 0.0f};
                accel = 150.0f;
            }
            ship.special_velocity += force * dt;
            ship.special_velocity.x += accel * dt;
            ship.special.value().sprite.value().move({ship.special_velocity.x * dt, ship.special_velocity.y * dt});
            break;
        }

        case MISSILE_DOWN:
            ship.special.value().sprite.value().move({ship.special_velocity.x * dt, ship.special_velocity.y * dt});
            break;

        case STAR:
        {
            sf::Vector2f force(-(ship.special.value().sprite.value().getPosition() - ship.ship_body.sprite.value().getPosition()));
            if (force.length() > 0)
            {
                force = force.normalized() * 400.0f * dt;
            }
            ship.special_velocity += force;
            ship.special.value().sprite.value().move({ship.special_velocity.x * dt, ship.special_velocity.y * dt});
            ship.special_lifespan += dt;
            if (ship.special_lifespan >= SPECIAL_BALL_LIFESPAN)
            {
                ship.special.reset();
            }
            break;
        }
    }
}

} // namespace GameObjects
} // namespace js

#endif
