#ifndef ASSETS_H
#define ASSETS_H

#include <vector>
#include <map>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Font.hpp>

enum SoundTypes
{
    DAMAGE,
    HIT_ENEMY,
    HIT_SHIP,
    BOOM,
    BOOM_BASE,
    GET_ITEM,
    CANNON_SHOT,
    SPECIAL_LAUNCH,
    BALL_BOUNCE,
    ENEMY_01_SHOT,
    ENEMY_02_SHOT,
    ENEMY_03_13_SHOT,
    ENEMY_05_SHOT,
    ENEMY_06_SHOT,
    ENEMY_07_SHOT,
    ENEMY_08_SHOT,
    ENEMY_09_LAUNCH,
    ENEMY_09_SHOT,
    ENEMY_10_SPAWN,
    DEATH,
    LAST,
};

struct Assets
{
    std::vector<sf::Texture> walls;
    std::vector<sf::Texture> items;
    std::vector<sf::Texture> enemies;
    std::map<int, sf::Texture> enemy_shots;
    sf::Texture ship;
    sf::Texture ship_flame_down_big;
    sf::Texture ship_flame_down_small;
    sf::Texture ship_flame_back;
    sf::Texture ship_cannon;
    sf::Texture special_ball;
    sf::Texture special_missile_side;
    sf::Texture special_missile_down;
    sf::Texture special_star;
    sf::Texture explosion;
    sf::Texture fragment;
    sf::Texture infobar;
    sf::Texture loadscr;
    std::map<SoundTypes, sf::SoundBuffer> sounds;
    sf::Font font;
};

void load_wall_textures(Assets & assets);
void load_item_textures(Assets & assets);
void load_enemy_textures(Assets & assets);
void load_enemy_shot_textures(Assets & assets);
void load_ship_textures(Assets & assets);
void load_misc_textures(Assets & assets);
void load_sounds(Assets & assets);
void load_font(Assets & assets);

#endif
