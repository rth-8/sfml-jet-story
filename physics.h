#ifndef PHYSICS_H
#define PHYSICS_H

bool checkCollision(
    const sf::Sprite & moving, const sf::Vector2f & movingHalfSize, const sf::Vector2f & movingPrevPos,
    const sf::Sprite & still, const sf::Vector2f & stillHalfSize, 
    sf::Vector2f & vec)
{
    float pox = movingHalfSize.x + stillHalfSize.x - abs(movingPrevPos.x - still.getPosition().x);
    float poy = movingHalfSize.y + stillHalfSize.y - abs(movingPrevPos.y - still.getPosition().y);

    float ox = movingHalfSize.x + stillHalfSize.x - abs(moving.getPosition().x - still.getPosition().x);
    float oy = movingHalfSize.y + stillHalfSize.y - abs(moving.getPosition().y - still.getPosition().y);

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
            if (moving.getPosition().y < still.getPosition().y) vec.y = -oy;
            if (moving.getPosition().y > still.getPosition().y) vec.y = oy;
        }
        else
        if (state == 2 || state == 3)
        {
            // horizontal
            if (moving.getPosition().x < still.getPosition().x) vec.x = -ox;
            if (moving.getPosition().x > still.getPosition().x) vec.x = ox;
        }
        
        return true;
    }
    
    return false;
}

#endif
