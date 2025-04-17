#ifndef PHYSICS_H
#define PHYSICS_H

bool checkCollision(const js::GameObjects::Animation & a1, const js::GameObjects::Animation & a2)
{
    const auto & pos1 = a1.sprite.value().getPosition() - a1.half_size;
    const auto & pos2 = a2.sprite.value().getPosition() - a2.half_size;

    if (pos1.x < pos2.x + a2.size.x && 
        pos1.x + a1.size.x > pos2.x && 
        pos1.y < pos2.y + a2.size.y &&
        pos1.y + a1.size.y > pos2.y)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool checkCollision(
    const sf::Vector2f & movingPos, const sf::Vector2f & movingHalfSize, const sf::Vector2f & movingPrevPos,
    const sf::Vector2f & stillPos, const sf::Vector2f & stillHalfSize, 
    sf::Vector2f & vec)
{
    float pox = movingHalfSize.x + stillHalfSize.x - abs(movingPrevPos.x - stillPos.x);
    float poy = movingHalfSize.y + stillHalfSize.y - abs(movingPrevPos.y - stillPos.y);

    float ox = movingHalfSize.x + stillHalfSize.x - abs(movingPos.x - stillPos.x);
    float oy = movingHalfSize.y + stillHalfSize.y - abs(movingPos.y - stillPos.y);

    int8_t state = 0;
    if (pox > 0 && poy <= 0) state = 1;
    else
    if (pox <= 0 && poy > 0) state = 2;
    else
    if (pox > 0 && poy > 0) state = 3;

    if (ox > 0 && oy > 0)
    {        
        if (state == 1)
        {
            // vertical            
            if (movingPos.y < stillPos.y) vec.y = -oy;
            if (movingPos.y > stillPos.y) vec.y = oy;
        }
        else
        if (state == 2 || state == 3)
        {
            // horizontal
            if (movingPos.x < stillPos.x) vec.x = -ox;
            if (movingPos.x > stillPos.x) vec.x = ox;
        }
        
        return true;
    }
    
    return false;
}

#endif
