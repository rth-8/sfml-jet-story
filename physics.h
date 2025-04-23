#ifndef PHYSICS_H
#define PHYSICS_H

#include "animation.h"

bool checkCollision(const Animation & a1, const Animation & a2);

bool checkCollision(
    const sf::Vector2f & movingPos, const sf::Vector2f & movingHalfSize, const sf::Vector2f & movingPrevPos,
    const sf::Vector2f & stillPos, const sf::Vector2f & stillHalfSize, 
    sf::Vector2f & vec);

#endif
