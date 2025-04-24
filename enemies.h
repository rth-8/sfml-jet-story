#ifndef ENEMIES_H
#define ENEMIES_H

#include "animation.h"

struct Enemy
{
    Animation anim;
    std::optional<Animation> carried_enemy;
    std::optional<Animation> carried_item;
    sf::Vector2f previous_position;
    sf::Vector2f velocity;
    int health;
    int carried_enemy_health;
    float shooting_counter;
    float shooting_speed;
    float shooting_delay;
};

struct Assets;
struct Room;
struct EnemyData;
struct MazeData;
struct Maze;
struct Ship;
struct Projectiles;
struct Explosions;
struct Sounds;

sf::Vector2f enemy_get_size(Enemy & enemy);
sf::Vector2f enemy_get_half_size(Enemy & enemy);
sf::Vector2f enemy_get_position(Enemy & enemy);

void create_enemy_anim(Animation & anim, const int & id, const int & subid, const Assets & assets);
void create_enemy(Enemy & eo, const EnemyData & ed, const MazeData & md, const Assets & assets);

void shooting_horizontal(Enemy & enemy, Animation & enemyAnim, Projectiles & projectiles, Explosions & explosions, Sounds & sounds, 
    const Ship & ship, const Assets & assets, float dt);

void shooting_vertical(Enemy & enemy, Animation & enemyAnim, Projectiles & projectiles, Sounds & sounds, 
    const Ship & ship, const Assets & assets, float dt);

void shooting_diagonal(Animation & enemyAnim, Projectiles & projectiles, Explosions & explosions, Sounds & sounds, 
    const Ship & ship, const Assets & assets, float dt);

void shooting_tracking(Enemy & enemy, Animation & enemyAnim, Projectiles & projectiles, Sounds & sounds, 
    const Ship & ship, const Assets & assets, float dt);

void move_enemy_mirror(Enemy & enemy, const Ship & ship);
void move_enemy_random_vertical_tracking(Enemy & enemy, const Ship & ship, float dt, int gFrame);
void move_enemy_random(Enemy & enemy, float dt, int gFrame);
void move_enemy_constant(Enemy & enemy, float dt);

void spawn_enemy(Room & room, const Enemy & spawner, Sounds & sounds, const MazeData & md, const Assets & assets, int gFrame);

void move_enemy_carried_element(Room & room, Enemy & enemy, Projectiles & projectiles, Explosions & explosions, Sounds & sounds, 
    const Ship & ship, const MazeData & md, const Assets & assets, float dt, int gFrame);

void move_enemy(Room & room, Enemy & enemy, Projectiles & projectiles, Explosions & explosions, Sounds & sounds, 
    const Ship & ship, const MazeData & md, const Assets & assets, float dt, int gFrame);

void collision_enemy_wall(Enemy & enemy, Animation & wall);
bool collision_enemy_ship(Enemy & enemy, Maze & maze, Ship & ship, Explosions & explosions, Sounds & sounds, const Assets & assets);
void collision_enemy_cannon(Enemy & enemy, Maze & maze, Ship & ship, Explosions & explosions, Sounds & sounds, const Assets & assets);
void collision_enemy_special(Enemy & enemy, Maze & maze, Ship & ship, Explosions & explosions, Sounds & sounds, const Assets & assets);

void enemy_check_bounds(Enemy & enemy);

#endif
