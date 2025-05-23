#ifndef ANIMATION_H
#define ANIMATION_H

#include <optional>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

struct Animation
{
    uint16_t id;
    uint16_t subid;
    std::optional<sf::Sprite> sprite;
    uint16_t color_index;
    sf::Vector2f size;
    sf::Vector2f half_size;
    int frame_count;
    int speed;
    int frame;
    int counter;
    bool isAlive;
};

void create_animation(Animation & anim, uint16_t id, const sf::Texture & tex, float fw, float fh, int fc = 1, int spd = 0);
void animation_update(Animation & anim);
void set_frame(Animation & anim, int aFrame);
bool has_animation_ended(const Animation & anim);

#endif
