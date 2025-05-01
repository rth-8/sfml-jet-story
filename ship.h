#ifndef SHIP_H
#define SHIP_H

#include "animation.h"

enum SpecialType
{
    NONE,
    BALL,
    MISSILE_SIDE,
    MISSILE_DOWN,
    STAR,
};

struct Ship
{
    Animation ship_body;
    Animation ship_flame_down_big;
    Animation ship_flame_down_small;
    Animation ship_flame_back;
    sf::Vector2f previous_position;
    sf::Vector2f velocity;
    uint16_t shield;
    float fuel;
    int damage_delay;

    std::optional<Animation> cannon;
    sf::Vector2f cannon_previous_position;
    sf::Vector2f cannon_velocity;
    uint16_t cannon_ammo;

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

struct Assets;
struct Maze;
struct Sounds;

void create_ship(Ship & ship, const Assets & assets);
void reset_ship(Ship & ship, const sf::Vector2f & pos);
void move_ship(float dt, Ship & ship);
void update_flames_positions(Ship & ship);
void get_item(Ship & ship, int id);
void create_cannon(Ship & ship, const Assets & assets);
void move_cannon(float dt, Ship & ship);
void create_special(Ship & ship, const Assets & assets);
void reset_special(Ship & ship);
void move_special(float dt, Ship & ship);
bool ship_check_bounds(Maze & maze, Ship & ship);
void cannon_check_bounds(Ship & ship);
void special_check_bounds(Ship & ship, Sounds & sounds);
void collision_ship_wall(Ship & ship, Animation & wall);
void collision_cannon_wall(Ship & ship, Animation & wall);
void collision_special_wall(Ship & ship, Animation & wall, Sounds & sounds);
void collision_ship_item(Ship & ship, Animation & item, Sounds & sounds);

bool load_ship(Ship & ship, int slot);
bool save_ship(Ship & ship, int slot);

#endif
