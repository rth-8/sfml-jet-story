#ifndef EXPLOSIONS_H
#define EXPLOSIONS_H

#include <vector>

#include <SFML/System/Vector2.hpp>

#include "assets.h"
#include "animation.h"

#define FLASH_MAX 10
#define FLASH_MID 5
#define FLASH_SPD 20

struct Fragment
{
    Animation anim;
    sf::Vector2f velocity;
};

struct Explosions
{
    std::vector<Animation> explosions;
    std::vector<Fragment> fragments;
    float flashing_counter;
};

void create_fragment(Explosions & explosions, const sf::Vector2f & pos, const sf::Vector2f & velocity, const Assets & assets);
void create_explosion(Explosions & explosions, const sf::Vector2f & pos, const Assets & assets);
void move_fragment(Fragment & fg, float dt);
void fragment_check_bounds(Fragment & fg);

#endif
