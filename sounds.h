#ifndef SOUNDS_H
#define SOUNDS_H

#include <map>
#include <SFML/Audio/Sound.hpp>
#include "assets.h"

struct Sounds
{
    std::map<SoundTypes, sf::Sound> sounds;
};

void create_sounds(Sounds & sounds, Assets & assets);

#endif
