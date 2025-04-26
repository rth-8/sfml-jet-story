#include "infobar.h"

#include <iostream>

#include "common.h"
#include "zx_colors.h"
#include "assets.h"
#include "maze.h"

#define BAR_RECT_INNER_SIZE 22
#define BAR_RECT_OUTER_SIZE 25
#define VALUE_BAR_WIDTH 97
#define BAR_FONT_SIZE 24

void create_rect_line(InfoBar & ib, int x, int y, int amount)
{
    for (int i=0; i<amount; ++i)
    {
        InfoBarRect rect;
        rect.rect.setSize({BAR_RECT_INNER_SIZE, BAR_RECT_INNER_SIZE});
        rect.rect.setPosition({static_cast<float>(x + i * BAR_RECT_OUTER_SIZE), static_cast<float>(y)});
        rect.rect.setFillColor(sf::Color::Cyan);
        rect.counter = 10;
        ib.rects.push_back(rect);
    }
}

void create_infobar(InfoBar & ib, const Assets & assets)
{
    ib.tex = std::make_optional<sf::Sprite>(assets.infobar);

    // row 1
    create_rect_line(ib, 0, 0, 32);
    // row 2
    create_rect_line(ib, 0, 25, 1);
    create_rect_line(ib, 250, 25, 1);
    create_rect_line(ib, 325, 25, 1);
    create_rect_line(ib, 500, 25, 2);
    create_rect_line(ib, 700, 25, 1);
    create_rect_line(ib, 775, 25, 1);
    // row 3
    create_rect_line(ib, 0, 50, 1);
    create_rect_line(ib, 250, 50, 1);
    create_rect_line(ib, 325, 50, 1);
    create_rect_line(ib, 500, 50, 9);
    create_rect_line(ib, 775, 50, 1);
    // row 4
    create_rect_line(ib, 0, 75, 32);

    ib.shield_rect.setSize({VALUE_BAR_WIDTH, BAR_RECT_INNER_SIZE});
    ib.shield_rect.setPosition({375.0f, 50.0f});
    ib.shield_rect.setFillColor(sf::Color::White);

    ib.fuel_rect.setSize({VALUE_BAR_WIDTH, BAR_RECT_INNER_SIZE});
    ib.fuel_rect.setPosition({125.0f, 25.0f});
    ib.fuel_rect.setFillColor(sf::Color::White);

    ib.ammo_rect.setSize({VALUE_BAR_WIDTH, BAR_RECT_INNER_SIZE});
    ib.ammo_rect.setPosition({125.0f, 50.0f});
    ib.ammo_rect.setFillColor(sf::Color::White);

    ib.special_type = NONE;

    ib.special_ammount = std::make_optional<sf::Text>(assets.font_infobar);
    ib.special_ammount.value().setPosition({275.0f, 50.0f - 2.0f});
    ib.special_ammount.value().setFillColor(sf::Color::White);
    ib.special_ammount.value().setCharacterSize(BAR_FONT_SIZE);

    ib.bases_ammount = std::make_optional<sf::Text>(assets.font_infobar);
    ib.bases_ammount.value().setPosition({725.0f, 50.0f - 2.0f});
    ib.bases_ammount.value().setFillColor(sf::Color::White);
    ib.bases_ammount.value().setCharacterSize(BAR_FONT_SIZE);

    ib.score = std::make_optional<sf::Text>(assets.font_infobar);
    ib.score.value().setPosition({550.0f, 25.0f - 2.0f});
    ib.score.value().setFillColor(sf::Color::White);
    ib.score.value().setCharacterSize(BAR_FONT_SIZE);
}

void update_infobar(InfoBar & ib, const Maze & maze, const Ship & ship, const Assets & assets, int gFrame)
{
    if (ib.special_type != ship.special_type)
    {
        switch (ship.special_type)
        {
            case BALL:
                if (!ib.special1.has_value()) ib.special1 = std::make_optional<sf::Sprite>(assets.special_ball);
                else ib.special1.value().setTexture(assets.special_ball, true);
                if (!ib.special2.has_value()) ib.special2 = std::make_optional<sf::Sprite>(assets.special_ball);
                else ib.special2.value().setTexture(assets.special_ball, true);
                break;
            case MISSILE_SIDE:
                if (!ib.special1.has_value()) ib.special1 = std::make_optional<sf::Sprite>(assets.special_missile_side);
                else ib.special1.value().setTexture(assets.special_missile_side, true);
                if (!ib.special2.has_value()) ib.special2 = std::make_optional<sf::Sprite>(assets.special_missile_side);
                else ib.special2.value().setTexture(assets.special_missile_side, true);
                break;
            case MISSILE_DOWN:
                if (!ib.special1.has_value()) ib.special1 = std::make_optional<sf::Sprite>(assets.special_missile_down);
                else ib.special1.value().setTexture(assets.special_missile_down, true);
                if (!ib.special2.has_value()) ib.special2 = std::make_optional<sf::Sprite>(assets.special_missile_down);
                else ib.special2.value().setTexture(assets.special_missile_down, true);
                break;
            case STAR:
                if (!ib.special1.has_value()) ib.special1 = std::make_optional<sf::Sprite>(assets.special_star);
                else ib.special1.value().setTexture(assets.special_star, true);
                if (!ib.special2.has_value()) ib.special2 = std::make_optional<sf::Sprite>(assets.special_star);
                else ib.special2.value().setTexture(assets.special_star, true);
                break;
                
            case NONE:
                std::cerr << "Infobar: Unexpected special type!";
                std::abort();
                break;
        }

        if (ib.special1.has_value())
        {
            ib.special1.value().setPosition({275.0f, 25.0f});
        }
        if (ib.special2.has_value())
        {
            ib.special2.value().setPosition({300.0f, 25.0f});
        }
    }

    ib.shield_rect.setSize({(float)ship.shield/HEALTH_MAX*VALUE_BAR_WIDTH, BAR_RECT_INNER_SIZE});
    ib.fuel_rect.setSize({(float)ship.fuel/FUEL_MAX*VALUE_BAR_WIDTH, BAR_RECT_INNER_SIZE});
    ib.ammo_rect.setSize({(float)ship.cannon_ammo/AMMO_MAX*VALUE_BAR_WIDTH, BAR_RECT_INNER_SIZE});

    if (ib.special_ammount.has_value())
    {
        ib.special_ammount.value().setString(std::format("{:02d}", ship.special_ammo));
    }
    if (ib.bases_ammount.has_value())
    {
        ib.bases_ammount.value().setString(std::format("{:02d}", maze.base_cnt));
    }
    if (ib.score.has_value())
    {
        ib.score.value().setString(std::format("{:06d}", ship.score));
    }

    if (gFrame % 10 == 0)
    {
        for (auto & rect : ib.rects)
        {
            if (std::rand()%4 == 0)
            {
                int idx = std::rand() % 16;
                if (idx == 0 || idx == 7 || idx == 8 || idx == 15)
                {
                    idx = 13;
                }
                rect.rect.setFillColor(zx_colors[idx]);
                rect.counter = 10;
            }
            else
            {
                rect.counter -= 1;
                if (rect.counter == 0) 
                {
                    rect.rect.setFillColor(sf::Color::Cyan);
                    rect.counter = 10;
                }
            }
        }
    }
}

void draw_infobar(sf::RenderWindow & window, InfoBar & ib)
{
    window.draw(ib.tex.value());

    for (auto & rect : ib.rects)
    {
        window.draw(rect.rect);
    }

    if (ib.special1.has_value())
    {
        window.draw(ib.special1.value());
    }
    if (ib.special2.has_value())
    {
        window.draw(ib.special2.value());
    }

    window.draw(ib.shield_rect);
    window.draw(ib.fuel_rect);
    window.draw(ib.ammo_rect);

    window.draw(ib.special_ammount.value());
    window.draw(ib.bases_ammount.value());
    window.draw(ib.score.value());
}
