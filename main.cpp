#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <iostream>
#include <vector>

#include "assets.h"
#include "maze.h"
#include "objects.h"
#include "physics.h"
#include "debug_draw.h"

#define EDGE_TOP 100
#define EDGE_BOTTOM 600
#define EDGE_LEFT 0
#define EDGE_RIGHT 800

#define ACCEL_VERT 20.0f
#define ACCEL_HORIZ 7.0f
#define FRICTION 0.7f
#define GRAVITY 8.9f

int main()
{
    sf::RenderWindow window(sf::VideoMode({800, 600}), "js");
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);

    sf::Clock clock;
    int game_frame = 0;

    Assets assets;
    load_wall_textures(assets);
    load_ship_textures(assets);
    
    js::Data::Maze maze;
    js::Data::load_maze(maze);
    
    js::GameObjects::Maze maze_o;
    js::GameObjects::create_maze(maze_o, maze, assets);
    
    js::GameObjects::Ship ship_o;
    js::GameObjects::create_ship(ship_o, {200,310}, assets);

    while (window.isOpen())
    {
        sf::Time dt = clock.restart();
        float dtAsSeconds = dt.asSeconds();

        auto & room_o = js::GameObjects::get_current_room(maze_o);

        // input

        ship_o.previous_position = ship_o.ship_body.sprite.value().getPosition();
        ship_o.thrust_up = false;
        ship_o.thrust_horiz = false;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
        {
            ship_o.velocity.y -= ACCEL_VERT * dtAsSeconds;
            ship_o.thrust_up = true;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
        {
            // launch special
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
        {
            ship_o.velocity.x -= ACCEL_HORIZ * dtAsSeconds;
            ship_o.ship_body.sprite.value().setScale({-1, 1});
            ship_o.thrust_horiz = true;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
        {
            ship_o.velocity.x += ACCEL_HORIZ * dtAsSeconds;
            ship_o.ship_body.sprite.value().setScale({1, 1});
            ship_o.thrust_horiz = true;
        }

        // update

        ship_o.velocity.x -= FRICTION * ship_o.velocity.x * dtAsSeconds;
        ship_o.velocity.y -= FRICTION * ship_o.velocity.y * dtAsSeconds;
        ship_o.velocity.y += GRAVITY * dtAsSeconds;
        ship_o.ship_body.sprite.value().move(ship_o.velocity);

        for (auto & wall_o : room_o.walls)
        {
            sf::Vector2f vec(0.0f, 0.0f);
            if (checkCollision(ship_o.ship_body.sprite.value(), ship_o.ship_body.half_size, ship_o.previous_position, 
                                wall_o.sprite.value(), wall_o.half_size, vec))
            {
                // resolution
                ship_o.ship_body.sprite.value().move(vec);
                if (vec.x != 0)
                {
                    ship_o.velocity.x = 0;
                }
                if (vec.y != 0)
                {
                    ship_o.velocity.y = 0;
                }
            }
        }

        if (ship_o.ship_body.sprite.value().getPosition().x < EDGE_LEFT + ship_o.ship_body.half_size.x)
        {
            // std::cout << "NEXT ROOM: left\n";
            maze_o.current_room_col--;
            ship_o.ship_body.sprite.value().setPosition({EDGE_RIGHT - ship_o.ship_body.half_size.x, ship_o.ship_body.sprite.value().getPosition().y});
        }
        else if (ship_o.ship_body.sprite.value().getPosition().x > EDGE_RIGHT - ship_o.ship_body.half_size.x)
        {
            // std::cout << "NEXT ROOM: right\n";
            maze_o.current_room_col++;
            ship_o.ship_body.sprite.value().setPosition({EDGE_LEFT + ship_o.ship_body.half_size.x, ship_o.ship_body.sprite.value().getPosition().y});
        }
        else if (ship_o.ship_body.sprite.value().getPosition().y < EDGE_TOP + ship_o.ship_body.half_size.y)
        {
            // std::cout << "NEXT ROOM: up\n";
            maze_o.current_room_row--;
            ship_o.ship_body.sprite.value().setPosition({ship_o.ship_body.sprite.value().getPosition().x, EDGE_BOTTOM - ship_o.ship_body.half_size.y});
        }
        else if (ship_o.ship_body.sprite.value().getPosition().y > EDGE_BOTTOM - ship_o.ship_body.half_size.y)
        {
            // std::cout << "NEXT ROOM: down\n";
            maze_o.current_room_row++;
            ship_o.ship_body.sprite.value().setPosition({ship_o.ship_body.sprite.value().getPosition().x, EDGE_TOP + ship_o.ship_body.half_size.y});
        }

        
        js::GameObjects::update_ship(ship_o);

        // animations

        if (ship_o.thrust_horiz)
        {
            js::GameObjects::animation_update(ship_o.ship_flame_back, game_frame);
        }
        if (ship_o.thrust_up) 
        {
            js::GameObjects::animation_update(ship_o.ship_flame_down_big, game_frame);
            js::GameObjects::animation_update(ship_o.ship_flame_down_small, game_frame);
        }

        // SFML events
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        window.clear();

        // render

        window.draw(ship_o.ship_body.sprite.value());
        if (ship_o.thrust_horiz)
        {
            window.draw(ship_o.ship_flame_back.sprite.value());
        }
        if (ship_o.thrust_up)
        {
            window.draw(ship_o.ship_flame_down_big.sprite.value());
            window.draw(ship_o.ship_flame_down_small.sprite.value());
        }
        // drawPoint(window, ship_o.ship_body.sprite.value().getPosition(), 10, sf::Color::Green);
        // drawBB(window, ship_o.ship_body, sf::Color::Green);

        for (auto & wall_o : room_o.walls)
        {
            window.draw(wall_o.sprite.value());
            // drawPoint(window, wall_o.sprite.value().getPosition(), 10, sf::Color::Green);
            // drawBB(window, wall_o, sf::Color::Green);
        }

        window.display();

        game_frame++;
    }

    return 0;
}
