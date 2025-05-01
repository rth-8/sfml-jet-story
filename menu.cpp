#include "menu.h"
#include "physics.h"

void create_button(Button & button, std::string && str, const sf::Vector2f & pos, const sf::Vector2f & size, 
    const sf::Font & font, const unsigned int & charSize)
{
    button.rect.setSize(size);
    button.rect.setPosition(pos);
    
    button.rect.setFillColor(sf::Color(0, 0, 0, 0));
    button.rect.setOutlineThickness(-4);

    button.text = std::make_optional<sf::Text>(font);
    button.text.value().setString(str);
    button.text.value().setCharacterSize(charSize);
    
    auto button_center = pos + size * 0.5f;
    auto text_half_size = button.text.value().getGlobalBounds().size * 0.5f;
    int tx = button_center.x - text_half_size.x;
    int ty = button_center.y - text_half_size.y;
    button.text.value().setPosition({(float)tx, (float)ty});

    button.enabled = true;
    button.selected = false;
}

void set_button_string(Button & button, std::string && str)
{
    button.text.value().setString(str);

    auto button_center = button.rect.getPosition() + button.rect.getSize() * 0.5f;
    auto text_half_size = button.text.value().getGlobalBounds().size * 0.5f;
    int tx = button_center.x - text_half_size.x;
    int ty = button_center.y - text_half_size.y;
    button.text.value().setPosition({(float)tx, (float)ty});
}

void draw_button(sf::RenderWindow & window, Button & button)
{
    if (button.enabled)
    {
        if (button.selected)
        {
            button.rect.setOutlineColor(sf::Color::Yellow);
        }
        else
        {
            button.rect.setOutlineColor(sf::Color::White);
        }
        button.text.value().setFillColor(sf::Color::White);
    }
    else
    {
        button.rect.setOutlineColor(sf::Color(100.0f, 100.0f, 100.0f));
        button.text.value().setFillColor(sf::Color(100.0f, 100.0f, 100.0f));
    }

    window.draw(button.rect);
    if (button.text.has_value())
    {
        window.draw(button.text.value());
    }
}

void add_button(Menu & menu, std::string && str, const sf::Vector2f & pos, const sf::Vector2f & size, 
    const sf::Font & font, const unsigned int & charSize)
{
    Button btn;
    create_button(btn, std::move(str), pos, size, font, charSize);
    menu.buttons.push_back(btn);
}

void draw_menu(sf::RenderWindow & window, Menu & menu)
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
                for (size_t i = menu.current-1; i >= 0; --i)
                {
                    if (menu.buttons[i].enabled)
                    {
                        menu.buttons[menu.current].selected = false;
                        menu.current = i;
                        menu.buttons[menu.current].selected = true;
                        break;
                    }
                }
            }
        }
        
        if (keyPressed->scancode == sf::Keyboard::Scancode::Down)
        {
            if (menu.current < menu.buttons.size() - 1)
            {
                for (size_t i = menu.current+1; i < menu.buttons.size(); ++i)
                {
                    if (menu.buttons[i].enabled)
                    {
                        menu.buttons[menu.current].selected = false;
                        menu.current = i;
                        menu.buttons[menu.current].selected = true;
                        break;
                    }
                }
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
                menu.buttons[i].selected = true;
                menu.current = i;
            }
            else
            {
                menu.buttons[i].selected = false;
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
