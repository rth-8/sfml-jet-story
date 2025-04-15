#ifndef ASSETS_H
#define ASSETS_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <format>

struct Assets
{
    std::vector<sf::Texture> walls;
    sf::Texture ship;
    sf::Texture ship_flame_down_big;
    sf::Texture ship_flame_down_small;
    sf::Texture ship_flame_back;
    sf::Texture ship_cannon;
    sf::Texture special_ball;
    sf::Texture special_missile_side;
    sf::Texture special_missile_down;
    sf::Texture special_star;
};

void load_wall_textures(Assets & assets)
{
    for (int8_t i=0; i<30; ++i)
    {
        assets.walls.push_back(sf::Texture(std::format("./images/walls/wall_{:02d}.png", i)));
    }
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

#endif
