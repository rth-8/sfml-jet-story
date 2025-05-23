#ifndef DEBUG_DRAW_H
#define DEBUG_DRAW_H

void drawPoint(sf::RenderWindow & window, const sf::Vector2f & point, int size, const sf::Color & color)
{
    std::array line1 =
    {
        sf::Vertex{sf::Vector2f(point.x - size, point.y), color},
        sf::Vertex{sf::Vector2f(point.x + size, point.y), color}
    };

    std::array line2 =
    {
        sf::Vertex{sf::Vector2f(point.x, point.y - size), color},
        sf::Vertex{sf::Vector2f(point.x, point.y + size), color}
    };

    window.draw(line1.data(), line1.size(), sf::PrimitiveType::Lines);
    window.draw(line2.data(), line2.size(), sf::PrimitiveType::Lines);
}

static void drawBB(sf::RenderWindow & window, 
                   const sf::Vector2f & pos, 
                   const sf::Vector2f & size,
                   const sf::Vector2f & half_size,
                   const sf::Color & color)
{
    sf::RectangleShape rectangle(size);
    rectangle.setPosition({pos.x - half_size.x, pos.y - half_size.y});
    rectangle.setFillColor(sf::Color(0, 0, 0, 0));
    rectangle.setOutlineThickness(-1);
    rectangle.setOutlineColor(color);
    window.draw(rectangle);
}

#endif
