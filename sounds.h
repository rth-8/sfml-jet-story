#ifndef SOUNDS_H
#define SOUNDS_H

std::optional<sf::Sound> sound_ship_damage;
std::optional<sf::Sound> sound_hit;
std::optional<sf::Sound> sound_boom;

void create_sounds(Assets & assets)
{
    sound_ship_damage = std::make_optional<sf::Sound>(assets.sounds.at(DAMAGE));
    sound_ship_damage.value().setLooping(true);

    sound_hit = std::make_optional<sf::Sound>(assets.sounds.at(HIT));

    sound_boom = std::make_optional<sf::Sound>(assets.sounds.at(BOOM));
}

#endif
