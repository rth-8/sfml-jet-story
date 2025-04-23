#ifndef PROJECTILES_H
#define PROJECTILES_H

#include <vector>

#include "animation.h"

struct Projectile
{
    Animation anim;
    sf::Vector2f velocity;
};

struct Projectiles
{
    std::vector<Projectile> projectiles;
};

struct Assets;
struct Ship;
struct Explosions;
struct Sounds;

Projectile & create_projectile(Projectiles & projectiles, Sounds & sounds, const Assets & assets, int id);
void move_projectile(Projectile & prj, Sounds & sounds, float dt);
void collision_projectile_wall(Projectile & prj, Animation & wall, Explosions & explosions, Sounds & sounds, const Assets & assets);
void collision_projectile_ship(Projectile & prj, Ship & ship, Explosions & explosions, Sounds & sounds, const Assets & assets);
void projectile_check_bounds(Projectile & prj, Sounds & sounds);

#endif
