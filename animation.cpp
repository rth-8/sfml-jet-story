#include <cmath>

#include "animation.h"

void create_animation(Animation & anim, uint16_t id, const sf::Texture & tex, float fw, float fh, int fc, int spd)
{
    anim.id = id;
    anim.subid = 0;
    // frame size
    anim.size.x = fw;
    anim.size.y = fh;
    // frame half-size
    anim.half_size.x = fw * 0.5f;
    anim.half_size.y = fh * 0.5f;
    anim.frame_count = fc;
    anim.speed = spd;
    anim.frame = 0;
    anim.counter = 0;
    anim.sprite = std::make_optional<sf::Sprite>(tex);
    anim.sprite.value().setTextureRect(
        sf::IntRect({anim.frame * static_cast<int>(anim.size.x), 0},
                    {static_cast<int>(floor(anim.size.x)), static_cast<int>(floor(anim.size.y))}));
    anim.sprite.value().setOrigin(anim.half_size);
    anim.color_index = 15;
    anim.isAlive = true;
}

void animation_update(Animation & anim)
{
    if (anim.frame_count > 1 && anim.speed > 0)
    {
        anim.frame = (anim.counter / anim.speed) % anim.frame_count;
        anim.sprite.value().setTextureRect(
            sf::IntRect({anim.frame * static_cast<int>(floor(anim.size.x)), 0}, 
                        {static_cast<int>(floor(anim.size.x)), static_cast<int>(floor(anim.size.y))}));
        anim.counter++;
    }
}

void set_frame(Animation & anim, int aFrame)
{
    anim.frame = aFrame;
    anim.sprite.value().setTextureRect(
        sf::IntRect({anim.frame * static_cast<int>(anim.size.x), 0}, 
                    {static_cast<int>(anim.size.x), static_cast<int>(anim.size.y)}));
}

bool has_animation_ended(const Animation & anim)
{
    return (anim.frame == anim.frame_count - 1); // animation reached last frame
}
