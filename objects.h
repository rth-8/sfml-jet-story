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
    std::vector<Animation> items;
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
    int shield;
    int fuel;

    std::optional<Animation> cannon;
    sf::Vector2f cannon_previous_position;
    sf::Vector2f cannon_velocity;
    int cannon_ammo;

    std::optional<Animation> special;
    SpecialType special_type;
    int next_special;
    sf::Vector2f special_previous_position;
    sf::Vector2f special_velocity;
    uint8_t special_ammo;
    float special_lifespan;
    
    bool thrust_up;
    bool thrust_horiz;
};

#define HEALTH_MAX 1000.0f
#define FUEL_MAX 1000.0f
#define AMMO_MAX 1000

void create_ship(Ship & ship, const sf::Vector2f & pos, const Assets & assets)
{
    create_animation(ship.ship_body, 0, assets.ship, assets.ship.getSize().x, assets.ship.getSize().y);
    ship.ship_body.sprite.value().setPosition(pos);

    create_animation(ship.ship_flame_back, 0, assets.ship_flame_back, 43, 35, 2, 4);
    create_animation(ship.ship_flame_down_big, 0, assets.ship_flame_down_big, 32, 44, 2, 4);
    create_animation(ship.ship_flame_down_small, 0, assets.ship_flame_down_small, 19, 38, 2, 4);

    ship.previous_position = pos;
    ship.velocity = {0.0f, 0.0f};

    ship.shield = HEALTH_MAX;
    ship.fuel = FUEL_MAX;
    ship.cannon_ammo = AMMO_MAX;

    ship.special_type = SpecialType::BALL;
    ship.next_special = INT_MAX;
    ship.special_ammo = 4;

    ship.thrust_up = false;
    ship.thrust_horiz = false;
}

#define FRICTION 0.7f
#define GRAVITY 8.9f

void move_ship(float dt, Ship & ship)
{
    ship.velocity.x -= FRICTION * ship.velocity.x * dt;
    ship.velocity.y -= FRICTION * ship.velocity.y * dt;
    ship.velocity.y += GRAVITY * dt;
    ship.ship_body.sprite.value().move(ship.velocity);
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

void get_item(Ship & ship, int id)
{
    switch (id)
    {
        case 0:
            ship.cannon_ammo = AMMO_MAX;
            break;
        case 1:
            ship.special_type = js::GameObjects::BALL;
            ship.special_ammo = 20;
            break;
        case 2:
            ship.fuel = FUEL_MAX;
            break;
        case 3:
            ship.special_type = js::GameObjects::MISSILE_DOWN;
            ship.special_ammo = 50;
            break;
        case 4:
            ship.special_type = js::GameObjects::MISSILE_SIDE;
            ship.special_ammo = 50;
            break;
        case 5:
            ship.shield = HEALTH_MAX;
            break;
        case 6:
            ship.special_type = js::GameObjects::STAR;
            ship.special_ammo = 20;
            break;
    }
}

void create_cannon(Ship & ship, const Assets & assets)
{
    ship.cannon = std::make_optional<Animation>();
    create_animation(ship.cannon.value(), 0, assets.ship_cannon, assets.ship_cannon.getSize().x, assets.ship_cannon.getSize().y);
    ship.cannon.value().sprite.value().setPosition(ship.ship_body.sprite.value().getPosition());
    ship.cannon_previous_position = ship.cannon.value().sprite.value().getPosition();
    ship.cannon_velocity = {ship.ship_body.sprite.value().getScale().x * 600.0f, 0.0f};
}

void move_cannon(float dt, Ship & ship)
{
    ship.cannon.value().sprite.value().move({ship.cannon_velocity.x * dt, ship.cannon_velocity.y * dt});
}

void create_special(Ship & ship, const Assets & assets)
{
    ship.special = std::make_optional<Animation>();
    
    switch (ship.special_type)
    {
        case BALL:
            create_animation(ship.special.value(), BALL, assets.special_ball, assets.special_ball.getSize().x, assets.special_ball.getSize().y);
            ship.special_velocity = {250.0f, 250.0f};
            break;

        case MISSILE_SIDE:
            create_animation(ship.special.value(), MISSILE_SIDE, assets.special_missile_side, 
                assets.special_missile_side.getSize().x, assets.special_missile_side.getSize().y);
            ship.special.value().sprite.value().setScale(ship.ship_body.sprite.value().getScale());
            ship.special.value().sprite.value().setColor(sf::Color::Cyan);
            ship.special_velocity = {ship.special.value().sprite.value().getScale().x * 100.0f, 200.0f};
            break;

        case MISSILE_DOWN:
            create_animation(ship.special.value(), MISSILE_DOWN, assets.special_missile_down, 
                assets.special_missile_down.getSize().x, assets.special_missile_down.getSize().y);
            ship.special.value().sprite.value().setColor(sf::Color::Cyan);
            ship.special_velocity = {ship.velocity.x * 20.0f, 300.0f};
            break;

        case STAR:
            create_animation(ship.special.value(), STAR, assets.special_star, assets.special_star.getSize().x, assets.special_star.getSize().y);
            break;
    }

    ship.special.value().sprite.value().setPosition(ship.ship_body.sprite.value().getPosition());
    ship.special_previous_position = ship.special.value().sprite.value().getPosition();
    ship.special_lifespan = 0;
}

void reset_special(Ship & ship)
{
    ship.special.reset();
    if (ship.next_special < INT_MAX)
    {
        get_item(ship, ship.next_special);
        ship.next_special = INT_MAX;
    }
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
                reset_special(ship);
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
            force += {ship.special.value().sprite.value().getScale().x * 100.0f, 0.0f};
            accel = ship.special.value().sprite.value().getScale().x * 150.0f;
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
                reset_special(ship);
            }
            break;
        }
    }
}

} // namespace GameObjects
} // namespace js

#endif
