#ifndef MENU_H
#define MENU_H

#include <vector>
#include <optional>
#include <functional>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

struct Button
{
    sf::RectangleShape rect;
    std::optional<sf::Text> text;
    std::function<void()> callback;
};

struct Menu
{
    std::vector<Button> buttons;
    size_t current;
};

void create_button(Button & button, std::string && str, const sf::Vector2f & pos, const sf::Vector2f & size, 
    const sf::Font & font, const unsigned int & charSize);
void draw_button(sf::RenderWindow & window, const Button & button);
void add_button(Menu & menu, std::string && str, const sf::Vector2f & pos, const sf::Vector2f & size, 
    const sf::Font & font, const unsigned int & charSize);
void draw_menu(sf::RenderWindow & window, const Menu & menu);
void menu_input(Menu & menu, const std::optional<sf::Event> event);

#endif
