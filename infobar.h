#ifndef INFOBAR_H
#define INFOBAR_H

#include <optional>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

#include "ship.h"

struct InfoBarRect
{
    sf::RectangleShape rect;
    int counter;
};

struct InfoBar
{
    std::optional<sf::Sprite> tex;
    std::vector<InfoBarRect> rects;

    sf::RectangleShape shield_rect;
    sf::RectangleShape fuel_rect;
    sf::RectangleShape ammo_rect;

    std::optional<sf::Sprite> special1;
    std::optional<sf::Sprite> special2;
    SpecialType special_type;

    std::optional<sf::Text> special_ammount;
    std::optional<sf::Text> bases_ammount;
    std::optional<sf::Text> score;
};

struct Assets;
struct Maze;
struct Ship;

void create_rect_line(InfoBar & ib, int x, int y, int amount);
void create_infobar(InfoBar & ib, const Assets & assets);
void update_infobar(InfoBar & ib, const Maze & maze, const Ship & ship, const Assets & assets, int gFrame);
void draw_infobar(sf::RenderWindow & window, InfoBar & ib);

#endif
