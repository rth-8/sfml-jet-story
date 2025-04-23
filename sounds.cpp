#include "sounds.h"

void create_sounds(Sounds & sounds, Assets & assets)
{
    for (int i = DAMAGE; i != LAST; i++)
    {
        SoundTypes t = static_cast<SoundTypes>(i);
        sounds.sounds.insert({t, sf::Sound(assets.sounds.at(t))});
    }
    sounds.sounds.at(DAMAGE).setLooping(true);
    sounds.sounds.at(ENEMY_08_SHOT).setLooping(true);
    sounds.sounds.at(ENEMY_09_SHOT).setLooping(true);
}
