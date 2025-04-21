#ifndef SOUNDS_H
#define SOUNDS_H

std::map<SoundTypes, sf::Sound> sounds;

void create_sounds(Assets & assets)
{
    for (int i = DAMAGE; i != LAST; i++)
    {
        SoundTypes t = static_cast<SoundTypes>(i);
        sounds.insert({t, sf::Sound(assets.sounds.at(t))});
    }
    sounds.at(DAMAGE).setLooping(true);
    sounds.at(ENEMY_08_SHOT).setLooping(true);
    sounds.at(ENEMY_09_SHOT).setLooping(true);
}

#endif
