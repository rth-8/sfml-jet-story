#ifndef ASSETS_H
#define ASSETS_H

#include <map>

enum SoundTypes
{
    DAMAGE,
    HIT,
    BOOM,
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
    std::map<SoundTypes, sf::SoundBuffer> sounds;
};

void load_wall_textures(Assets & assets)
{
    for (int8_t i=0; i<30; ++i)
    {
        assets.walls.push_back(sf::Texture(std::format("./images/walls/wall_{:02d}.png", i)));
    }
}

void load_item_textures(Assets & assets)
{
    assets.items.push_back(sf::Texture("./images/items/item_ammo.png"));    // 0
    assets.items.push_back(sf::Texture("./images/items/item_ball.png"));    // 1
    assets.items.push_back(sf::Texture("./images/items/item_fuel.png"));    // 2
    assets.items.push_back(sf::Texture("./images/items/item_m_d.png"));     // 3
    assets.items.push_back(sf::Texture("./images/items/item_m_lr.png"));    // 4
    assets.items.push_back(sf::Texture("./images/items/item_shield.png"));  // 5
    assets.items.push_back(sf::Texture("./images/items/item_star.png"));    // 6
    assets.items.push_back(sf::Texture("./images/items/item_random.png"));  // 7
}

void load_enemy_textures(Assets & assets)
{
    for (int8_t i=0; i<22; ++i)
    {
        assets.enemies.push_back(sf::Texture(std::format("./images/enemies/enemy_{:02d}.png", i)));
    }
}

void load_enemy_shot_textures(Assets & assets)
{
    assets.enemy_shots.insert({1, sf::Texture("./images/enemies/enemy_01_shot.png")});
    assets.enemy_shots.insert({2, sf::Texture("./images/enemies/enemy_02_shot.png")});
    assets.enemy_shots.insert({3, sf::Texture("./images/enemies/enemy_03_shot.png")});
    assets.enemy_shots.insert({5, sf::Texture("./images/enemies/enemy_05_shot.png")});
    assets.enemy_shots.insert({6, sf::Texture("./images/enemies/enemy_06_shot.png")});
    assets.enemy_shots.insert({7, sf::Texture("./images/enemies/enemy_07_shot.png")});
    assets.enemy_shots.insert({8, sf::Texture("./images/enemies/enemy_08_shot.png")});
    assets.enemy_shots.insert({9, sf::Texture("./images/enemies/enemy_09_shot.png")});
    assets.enemy_shots.insert({13, sf::Texture("./images/enemies/enemy_13_shot.png")});
}

void load_ship_textures(Assets & assets)
{
    if (!assets.ship.loadFromFile("./images/ship/ship.png")) std::cerr << "Failed to load ship.png!";
    if (!assets.ship_flame_down_big.loadFromFile("./images/ship/flame_down_big.png")) std::cerr << "Failed to load flame_down_big.png!";
    if (!assets.ship_flame_down_small.loadFromFile("./images/ship/flame_down_small.png")) std::cerr << "Failed to load flame_down_small.png!";
    if (!assets.ship_flame_back.loadFromFile("./images/ship/flame_back.png")) std::cerr << "Failed to load flame_back.png!";
    if (!assets.ship_cannon.loadFromFile("./images/ship/cannon.png")) std::cerr << "Failed to load cannon.png!";
    if (!assets.special_ball.loadFromFile("./images/ship/ball.png")) std::cerr << "Failed to load ball.png!";
    if (!assets.special_missile_side.loadFromFile("./images/ship/missile_side.png")) std::cerr << "Failed to load missile_side.png!";
    if (!assets.special_missile_down.loadFromFile("./images/ship/missile_down.png")) std::cerr << "Failed to load missile_down.png!";
    if (!assets.special_star.loadFromFile("./images/ship/star.png")) std::cerr << "Failed to load star.png!";
}

void load_sounds(Assets & assets)
{
    assets.sounds.insert({DAMAGE, sf::SoundBuffer("./sounds/damage.wav")});
    assets.sounds.insert({HIT, sf::SoundBuffer("./sounds/enemy_damage.wav")});
    assets.sounds.insert({BOOM, sf::SoundBuffer("./sounds/boom.wav")});
}

#endif
