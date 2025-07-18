#include <iostream>
#include <fstream>

#include "ship.h"
#include "assets.h"
#include "maze.h"
#include "common.h"
#include "physics.h"
#include "sounds.h"

#define SPECIAL_BALL_LIFESPAN 12
#define SPECIAL_STAR_LIFESPAN 12

void create_ship(Ship & ship, const Assets & assets)
{
    create_animation(ship.ship_body, (uint16_t)0, assets.ship, assets.ship.getSize().x, assets.ship.getSize().y);
    create_animation(ship.ship_flame_back, (uint16_t)0, assets.ship_flame_back, 43, 35, 2, 4);
    create_animation(ship.ship_flame_down_big, (uint16_t)0, assets.ship_flame_down_big, 32, 44, 2, 4);
    create_animation(ship.ship_flame_down_small, (uint16_t)0, assets.ship_flame_down_small, 19, 38, 2, 4);
}

void reset_ship(Ship & ship, const sf::Vector2f & pos)
{
    ship.ship_body.sprite.value().setPosition(pos);
    ship.previous_position = pos;
    ship.velocity = {0.0f, 0.0f};
    ship.shield = HEALTH_MAX;
    ship.fuel = FUEL_MAX;
    ship.damage_delay = 0;
    ship.cannon_ammo = AMMO_MAX;
    ship.special_type = SpecialType::BALL;
    ship.next_special = INT_MAX;
    ship.special_ammo = 4;

    ship.thrust_up = false;
    ship.thrust_horiz = false;

    if (ship.cannon.has_value())
    {
        ship.cannon.reset();
    }
    if (ship.special.has_value())
    {
        ship.special.reset();
    }
}

void move_ship(float dt, Ship & ship)
{
    ship.velocity.x -= FRICTION * ship.velocity.x * dt;
    ship.velocity.y -= FRICTION * ship.velocity.y * dt;
    ship.velocity.y += GRAVITY * dt;
    ship.ship_body.sprite.value().move(ship.velocity);
}

void update_flames_positions(Ship & ship)
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
            ship.special_type = BALL;
            ship.special_ammo = 20;
            break;
        case 2:
            ship.fuel = FUEL_MAX;
            break;
        case 3:
            ship.special_type = MISSILE_DOWN;
            ship.special_ammo = 50;
            break;
        case 4:
            ship.special_type = MISSILE_SIDE;
            ship.special_ammo = 50;
            break;
        case 5:
            ship.shield = HEALTH_MAX;
            break;
        case 6:
            ship.special_type = STAR;
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
            ship.special_velocity = {ship.ship_body.sprite.value().getScale().x * 250.0f, 250.0f};
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
            
        case NONE:
            std::cerr << "Create Special: Unexpected special type!";
            std::abort();
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
        
        case NONE:
            std::cerr << "Move Special: Unexpected special type!";
            std::abort();
            break;
    }
}

void ship_next_room(Ship & ship)
{
    if (ship.cannon.has_value())
    {
        ship.cannon.reset();
    }
    if (ship.special.has_value())
    {
        switch (ship.special_type)
        {
            case BALL:
            case STAR:
                // ball moves with ship to next room
                ship.special.value().sprite.value().setPosition(ship.ship_body.sprite.value().getPosition());
                break;
            default:
                // missiles are destroyed
                reset_special(ship);
                break;
        }
    }
}

bool ship_check_bounds(Maze & maze, Ship & ship)
{
    if (ship.ship_body.sprite.value().getPosition().x < EDGE_LEFT + ship.ship_body.half_size.x)
    {
        // std::cout << "NEXT ROOM: left\n";
        maze.current_room_col--;
        get_current_room(maze).visited = true;
        ship.ship_body.sprite.value().setPosition({EDGE_RIGHT - ship.ship_body.half_size.x, ship.ship_body.sprite.value().getPosition().y});
        ship_next_room(ship);
        return true;
    }
    else if (ship.ship_body.sprite.value().getPosition().x > EDGE_RIGHT - ship.ship_body.half_size.x)
    {
        // std::cout << "NEXT ROOM: right\n";
        maze.current_room_col++;
        get_current_room(maze).visited = true;
        ship.ship_body.sprite.value().setPosition({EDGE_LEFT + ship.ship_body.half_size.x, ship.ship_body.sprite.value().getPosition().y});
        ship_next_room(ship);
        return true;
    }
    else if (ship.ship_body.sprite.value().getPosition().y < EDGE_TOP + ship.ship_body.half_size.y)
    {
        // std::cout << "NEXT ROOM: up\n";
        maze.current_room_row--;
        get_current_room(maze).visited = true;
        ship.ship_body.sprite.value().setPosition({ship.ship_body.sprite.value().getPosition().x, EDGE_BOTTOM - ship.ship_body.half_size.y});
        ship.previous_position = ship.ship_body.sprite.value().getPosition();
        ship_next_room(ship);
        return true;
    }
    else if (ship.ship_body.sprite.value().getPosition().y > EDGE_BOTTOM - ship.ship_body.half_size.y)
    {
        // std::cout << "NEXT ROOM: down\n";
        maze.current_room_row++;
        get_current_room(maze).visited = true;
        ship.ship_body.sprite.value().setPosition({ship.ship_body.sprite.value().getPosition().x, EDGE_TOP + ship.ship_body.half_size.y});
        ship_next_room(ship);
        return true;
    }

    return false;
}

void cannon_check_bounds(Ship & ship)
{
    if (ship.cannon.has_value())
    {
        if (ship.cannon.value().sprite.value().getPosition().x < EDGE_LEFT + ship.cannon.value().half_size.x ||
            ship.cannon.value().sprite.value().getPosition().x > EDGE_RIGHT - ship.cannon.value().half_size.x ||
            ship.cannon.value().sprite.value().getPosition().y < EDGE_TOP + ship.cannon.value().half_size.y ||
            ship.cannon.value().sprite.value().getPosition().y > EDGE_BOTTOM - ship.cannon.value().half_size.y)
        {
            ship.cannon.reset();
        }
    }
}

void special_check_bounds(Ship & ship, Sounds & sounds)
{
    if (ship.special.has_value())
    {
        auto & spc = ship.special.value();
        if (spc.sprite.value().getPosition().x < EDGE_LEFT + spc.half_size.x ||
            spc.sprite.value().getPosition().x > EDGE_RIGHT - spc.half_size.x)
        {
            switch (ship.special_type)
            {
                case BALL:
                case STAR:
                    // ball and start bounce of screen bounds
                    ship.special_velocity.x *= -1;
                    sounds.sounds.at(BALL_BOUNCE).stop();
                    sounds.sounds.at(BALL_BOUNCE).play();
                    break;
                default:
                    // missiles are destroyed
                    reset_special(ship);
                    break;
            }
        }
        else 
        if (spc.sprite.value().getPosition().y < EDGE_TOP + spc.half_size.y ||
            spc.sprite.value().getPosition().y > EDGE_BOTTOM - spc.half_size.y)
        {
            switch (ship.special_type)
            {
                case BALL:
                case STAR:
                    // ball and start bounce of screen bounds
                    ship.special_velocity.y *= -1;
                    break;
                default:
                    // missiles are destroyed
                    reset_special(ship);
                    break;
            }
        }
    }
}

void collision_ship_wall(Ship & ship, Animation & wall)
{
    sf::Vector2f vec(0.0f, 0.0f);

    if (checkCollision(ship.ship_body.sprite.value().getPosition(), ship.ship_body.half_size, ship.previous_position, 
                        wall.sprite.value().getPosition(), wall.half_size, 
                        vec))
    {
        // resolution
        ship.ship_body.sprite.value().move(vec);
        if (vec.x != 0)
        {
            ship.velocity.x = 0;
        }
        if (vec.y != 0)
        {
            ship.velocity.y = 0;
        }
    }
}

void collision_cannon_wall(Ship & ship, Animation & wall)
{
    if (ship.cannon.has_value())
    {
        if (checkCollision(ship.cannon.value(), wall))
        {
            ship.cannon.reset();
        }
    }
}

void collision_special_wall(Ship & ship, Animation & wall, Sounds & sounds)
{
    sf::Vector2f vec(0.0f, 0.0f);

    if (ship.special.has_value())
    {
        if (checkCollision(ship.special.value().sprite.value().getPosition(), ship.special.value().half_size, ship.special_previous_position, 
                            wall.sprite.value().getPosition(), wall.half_size, 
                            vec))
        {
            switch (ship.special_type)
            {
                case BALL:
                case STAR:
                    // ball and star bounce of walls
                    ship.special.value().sprite.value().move(vec);
                    if (vec.x != 0)
                    {
                        ship.special_velocity.x *= -1;
                    }
                    if (vec.y != 0)
                    {
                        ship.special_velocity.y *= -1;
                    }
                    sounds.sounds.at(BALL_BOUNCE).stop();
                    sounds.sounds.at(BALL_BOUNCE).play();
                    break;
                default:
                    // missiles are destroyed
                    reset_special(ship);
                    break;
            }
        }
    }
}

void collision_ship_item(Ship & ship, Animation & item, Sounds & sounds)
{
    if (checkCollision(ship.ship_body, item))
    {
        int id = item.id;
        if (item.id == 7) 
            id = std::rand() % 7;
        
        if (ship.special.has_value() && (id == 1 || id == 3 || id == 4 || id == 6))
        {
            ship.next_special = id;
        }
        else
        {
            get_item(ship, id);
        }
        item.isAlive = false;
        sounds.sounds.at(GET_ITEM).play();
    }
}

bool load_ship(Ship & ship, int slot)
{
    // std::cout << "Load ship: " << slot << "\n";

    std::ifstream input;
    input.open(std::format("./save{:03d}s", slot), std::ios::binary);
    if (input.is_open())
    {
        float x, y;
        input.read(reinterpret_cast <char*>(&x), sizeof(float));
        input.read(reinterpret_cast <char*>(&y), sizeof(float));
        ship.ship_body.sprite.value().setPosition({x, y});

        input.read(reinterpret_cast <char*>(&ship.shield), sizeof(uint16_t));
        input.read(reinterpret_cast <char*>(&ship.fuel), sizeof(float));
        input.read(reinterpret_cast <char*>(&ship.cannon_ammo), sizeof(uint16_t));
        input.read(reinterpret_cast <char*>(&ship.special_type), sizeof(SpecialType));
        input.read(reinterpret_cast <char*>(&ship.special_ammo), sizeof(uint8_t));

        ship.previous_position = ship.ship_body.sprite.value().getPosition();
        ship.velocity = {0.0f, 0.0f};
        ship.damage_delay = 0;
        ship.next_special = INT_MAX;
        ship.thrust_up = false;
        ship.thrust_horiz = false;
        if (ship.cannon.has_value())
        {
            ship.cannon.reset();
        }
        if (ship.special.has_value())
        {
            ship.special.reset();
        }

        input.close();
        return true;
    }
    else
    {
        return false;
    }
}

bool save_ship(Ship & ship, int slot)
{
    // std::cout << "Save ship: " << slot << "\n";

    std::ofstream output;
    output.open(std::format("./save{:03d}s", slot), std::ios::binary);
    if (output.is_open())
    {
        float x = ship.ship_body.sprite.value().getPosition().x;
        float y = ship.ship_body.sprite.value().getPosition().y;

        output.write(reinterpret_cast <char*>(&x), sizeof(float));
        output.write(reinterpret_cast <char*>(&y), sizeof(float));

        output.write(reinterpret_cast <char*>(&ship.shield), sizeof(uint16_t));
        output.write(reinterpret_cast <char*>(&ship.fuel), sizeof(float));
        output.write(reinterpret_cast <char*>(&ship.cannon_ammo), sizeof(uint16_t));
        output.write(reinterpret_cast <char*>(&ship.special_type), sizeof(SpecialType));
        output.write(reinterpret_cast <char*>(&ship.special_ammo), sizeof(uint8_t));

        output.close();
        return true;
    }
    else
    {
        return false;
    }
}
