#ifndef ANIMATION_H
#define ANIMATION_H

namespace js {
namespace GameObjects {

struct Animation
{
    int id;
    std::optional<sf::Sprite> sprite;
    int color_index;
    sf::Vector2f size;
    sf::Vector2f half_size;
    int frame_count;
    int speed;
    int frame;
    bool isAlive;
};

void create_animation(Animation & anim, int id, const sf::Texture & tex, float fw, float fh, int fc = 1, int spd = 0)
{
    anim.id = id;
    // frame size
    anim.size.x = fw;
    anim.size.y = fh;
    // frame half-size
    anim.half_size.x = fw * 0.5f;
    anim.half_size.y = fh * 0.5f;
    anim.frame_count = fc;
    anim.speed = spd;
    anim.frame = 0;
    anim.sprite = std::make_optional<sf::Sprite>(tex);
    anim.sprite.value().setTextureRect(
        sf::IntRect({anim.frame * static_cast<int>(anim.size.x), 0},
                    {static_cast<int>(anim.size.x), static_cast<int>(anim.size.y)}));
    anim.sprite.value().setOrigin(anim.half_size);
    anim.color_index = 15;
    anim.isAlive = true;
}

void animation_update(Animation & anim, int gFrame)
{
    if (anim.frame_count > 1 && anim.speed > 0 && gFrame > 0)
    {
        anim.frame = (gFrame / anim.speed) % anim.frame_count;
        anim.sprite.value().setTextureRect(
            sf::IntRect({anim.frame * static_cast<int>(anim.size.x), 0}, 
                        {static_cast<int>(anim.size.x), static_cast<int>(anim.size.y)}));
    }
}

void set_frame(Animation & anim, int aFrame)
{
    anim.frame = aFrame;
    anim.sprite.value().setTextureRect(
        sf::IntRect({anim.frame * static_cast<int>(anim.size.x), 0}, 
                    {static_cast<int>(anim.size.x), static_cast<int>(anim.size.y)}));
}

} // namespace GameObjects
} // namespace js

#endif
