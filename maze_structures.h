#ifndef MAZE_STRUCTURES_H
#define MAZE_STRUCTURES_H

typedef struct
{
    Animation anim;
    std::optional<Animation> carried_enemy;
    std::optional<Animation> carried_item;
    sf::Vector2f previous_position;
    sf::Vector2f velocity;
    int health;
    int carried_enemy_health;
}
Enemy;

typedef struct
{
    std::vector<Animation> walls;
    std::vector<Animation> items;
    std::vector<Enemy> enemies;
} 
Room;

typedef struct
{
    int current_room_row = 0;
    int current_room_col = 0;
    std::vector<Room> rooms;
    int base_cnt = 0;
}
Maze;

enum SpecialType
{
    NONE,
    BALL,
    MISSILE_SIDE,
    MISSILE_DOWN,
    STAR,
};

#define HEALTH_MAX 1000
#define FUEL_MAX 1000
#define AMMO_MAX 1000
#define FUEL_SUB 8

typedef struct
{
    Animation ship_body;
    Animation ship_flame_down_big;
    Animation ship_flame_down_small;
    Animation ship_flame_back;
    sf::Vector2f previous_position;
    sf::Vector2f velocity;
    int shield;
    float fuel;
    int damage_delay;
    int score;

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
}
Ship;

#endif
