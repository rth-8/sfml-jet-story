#include "menu.h"
#include "physics.h"

void create_button(Button & button, std::string && str, const sf::Vector2f & pos, const sf::Vector2f & size, const sf::Font & font)
{
    button.rect.setSize(size);
    button.rect.setPosition(pos);
    
    auto button_center = pos + size * 0.5f;
    
    button.rect.setFillColor(sf::Color(0, 0, 0, 0));
    button.rect.setOutlineThickness(-2);
    button.rect.setOutlineColor(sf::Color::White);
    
    button.text = std::make_optional<sf::Text>(font);
    button.text.value().setString(str);
    
    auto text_half_size = button.text.value().getGlobalBounds().size * 0.5f;
    
    int tx = button_center.x - text_half_size.x;
    int ty = button_center.y - text_half_size.y;
    
    button.text.value().setPosition({(float)tx, (float)ty});
}

void draw_button(sf::RenderWindow & window, const Button & button)
{
    window.draw(button.rect);
    if (button.text.has_value())
    {
        window.draw(button.text.value());
    }
}

void add_button(Menu & menu, std::string && str, const sf::Vector2f & pos, const sf::Vector2f & size, const sf::Font & font)
{
    Button btn;
    create_button(btn, std::move(str), pos, size, font);
    menu.buttons.push_back(btn);
}

void draw_menu(sf::RenderWindow & window, const Menu & menu)
{
    for (auto & btn : menu.buttons)
    {
        draw_button(window, btn);
    }
}

void menu_input(Menu & menu, const std::optional<sf::Event> event)
{
    if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
    {
        if (keyPressed->scancode == sf::Keyboard::Scancode::Up)
        {
            if (menu.current > 0)
            {
                menu.buttons[menu.current].rect.setOutlineColor(sf::Color::White);
                menu.current--;
                menu.buttons[menu.current].rect.setOutlineColor(sf::Color::Yellow);
            }
        }
        
        if (keyPressed->scancode == sf::Keyboard::Scancode::Down)
        {
            if (menu.current < menu.buttons.size() - 1)
            {
                menu.buttons[menu.current].rect.setOutlineColor(sf::Color::White);
                menu.current++;
                menu.buttons[menu.current].rect.setOutlineColor(sf::Color::Yellow);
            }
        }
        
        if (keyPressed->scancode == sf::Keyboard::Scancode::Enter)
        {
            if (menu.buttons[menu.current].callback)
            {
                menu.buttons[menu.current].callback();
            }
        }
    }
    
    if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>())
    {
        for (size_t i=0; i<menu.buttons.size(); ++i)
        {
            if (point_in_rect(menu.buttons[i].rect, mouseMoved->position.x, mouseMoved->position.y))
            {
                menu.buttons[i].rect.setOutlineColor(sf::Color::Yellow);
                menu.current = i;
            }
            else
            {
                menu.buttons[i].rect.setOutlineColor(sf::Color::White);
            }
        }
    }
    
    if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
    {
        for (size_t i=0; i<menu.buttons.size(); ++i)
        {
            if (point_in_rect(menu.buttons[i].rect, mouseButtonPressed->position.x, mouseButtonPressed->position.y))
            {
                if (menu.buttons[menu.current].callback)
                {
                    menu.buttons[menu.current].callback();
                }
            }
        }
    }
}
