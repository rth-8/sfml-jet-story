#ifndef MAZE_STRUCTURES_H
#define MAZE_STRUCTURES_H

typedef struct
{
    Animation anim;
    std::optional<Animation> carried_enemy;
    std::optional<Animation> carried_item;
    sf::Vector2f previous_position;
    sf::Vector2f velocity;
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
}
Maze;

enum SpecialType
{
    BALL,
    MISSILE_SIDE,
    MISSILE_DOWN,
    STAR,
};

typedef struct
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
}
Ship;

#endif
