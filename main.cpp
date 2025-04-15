#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

#include "assets.h"
#include "maze.h"
#include "animation.h"
#include "objects.h"
#include "physics.h"
#include "debug_draw.h"

#define EDGE_TOP 100
#define EDGE_BOTTOM 600
#define EDGE_LEFT 0
#define EDGE_RIGHT 800

#define ACCEL_VERT 20.0f
#define ACCEL_HORIZ 7.0f

int main()
{
    std::srand(std::time({}));

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
        if (ship_o.cannon.has_value())
        {
            ship_o.cannon_previous_position = ship_o.cannon.value().sprite.value().getPosition();
        }
        if (ship_o.special.has_value()) 
        {
            ship_o.special_previous_position = ship_o.special.value().sprite.value().getPosition();
        }
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
            if (!ship_o.special.has_value() && ship_o.special_ammo > 0)
            {
                ship_o.special_ammo--;
                js::GameObjects::create_special(ship_o, assets);
            }
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

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        {
            if (!ship_o.cannon.has_value() && ship_o.cannon_ammo > 0)
            {
                ship_o.cannon_ammo--;
                js::GameObjects::create_cannon(ship_o, assets);
            }
        }

        // update

        js::GameObjects::move_ship(dtAsSeconds, ship_o);
        if (ship_o.cannon.has_value())
        {
            js::GameObjects::move_cannon(dtAsSeconds, ship_o);
        }
        if (ship_o.special.has_value())
        {
            js::GameObjects::move_special(dtAsSeconds, ship_o);
        }

        // collisions

        for (auto & wall_o : room_o.walls)
        {
            sf::Vector2f vec(0.0f, 0.0f);

            // ship vs. walls
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

            // cannon vs. walls
            if (ship_o.cannon.has_value())
            {
                vec.x = 0;
                vec.y = 0;
                if (checkCollision(ship_o.cannon.value().sprite.value(), ship_o.cannon.value().half_size, ship_o.cannon_previous_position, 
                                    wall_o.sprite.value(), wall_o.half_size, vec))
                {
                    ship_o.cannon.reset();
                }
            }

            // special vs. walls
            if (ship_o.special.has_value())
            {
                vec.x = 0;
                vec.y = 0;
                if (checkCollision(ship_o.special.value().sprite.value(), ship_o.special.value().half_size, ship_o.special_previous_position, 
                                    wall_o.sprite.value(), wall_o.half_size, vec))
                {
                    switch (ship_o.special_type)
                    {
                        case js::GameObjects::BALL:
                        case js::GameObjects::STAR:
                            // ball and star bounce of walls
                            ship_o.special.value().sprite.value().move(vec);
                            if (vec.x != 0)
                            {
                                ship_o.special_velocity.x *= -1;
                            }
                            if (vec.y != 0)
                            {
                                ship_o.special_velocity.y *= -1;
                            }
                            break;
                        default:
                            // missiles are destroyed
                            ship_o.special.reset();
                            break;
                    }
                }
            }
        }

        if (ship_o.ship_body.sprite.value().getPosition().x < EDGE_LEFT + ship_o.ship_body.half_size.x)
        {
            // std::cout << "NEXT ROOM: left\n";
            maze_o.current_room_col--;
            ship_o.ship_body.sprite.value().setPosition({EDGE_RIGHT - ship_o.ship_body.half_size.x, ship_o.ship_body.sprite.value().getPosition().y});
            if (ship_o.special.has_value())
            {
                switch (ship_o.special_type)
                {
                    case js::GameObjects::BALL:
                    case js::GameObjects::STAR:
                        // ball moves with ship to next room
                        ship_o.special.value().sprite.value().setPosition(ship_o.ship_body.sprite.value().getPosition());
                        break;
                    default:
                        // missiles are destroyed
                        ship_o.special.reset();
                        break;
                }
            }
        }
        else if (ship_o.ship_body.sprite.value().getPosition().x > EDGE_RIGHT - ship_o.ship_body.half_size.x)
        {
            // std::cout << "NEXT ROOM: right\n";
            maze_o.current_room_col++;
            ship_o.ship_body.sprite.value().setPosition({EDGE_LEFT + ship_o.ship_body.half_size.x, ship_o.ship_body.sprite.value().getPosition().y});
            if (ship_o.special.has_value())
            {
                switch (ship_o.special_type)
                {
                    case js::GameObjects::BALL:
                    case js::GameObjects::STAR:
                        // ball moves with ship to next room
                        ship_o.special.value().sprite.value().setPosition(ship_o.ship_body.sprite.value().getPosition());
                        break;
                    default:
                        // missiles are destroyed
                        ship_o.special.reset();
                        break;
                }
            }
        }
        else if (ship_o.ship_body.sprite.value().getPosition().y < EDGE_TOP + ship_o.ship_body.half_size.y)
        {
            // std::cout << "NEXT ROOM: up\n";
            maze_o.current_room_row--;
            ship_o.ship_body.sprite.value().setPosition({ship_o.ship_body.sprite.value().getPosition().x, EDGE_BOTTOM - ship_o.ship_body.half_size.y});
            if (ship_o.special.has_value())
            {
                switch (ship_o.special_type)
                {
                    case js::GameObjects::BALL:
                    case js::GameObjects::STAR:
                        // ball moves with ship to next room
                        ship_o.special.value().sprite.value().setPosition(ship_o.ship_body.sprite.value().getPosition());
                        break;
                    default:
                        // missiles are destroyed
                        ship_o.special.reset();
                        break;
                }
            }
        }
        else if (ship_o.ship_body.sprite.value().getPosition().y > EDGE_BOTTOM - ship_o.ship_body.half_size.y)
        {
            // std::cout << "NEXT ROOM: down\n";
            maze_o.current_room_row++;
            ship_o.ship_body.sprite.value().setPosition({ship_o.ship_body.sprite.value().getPosition().x, EDGE_TOP + ship_o.ship_body.half_size.y});
            if (ship_o.special.has_value())
            {
                switch (ship_o.special_type)
                {
                    case js::GameObjects::BALL:
                    case js::GameObjects::STAR:
                        // ball and star move with ship to next room
                        ship_o.special.value().sprite.value().setPosition(ship_o.ship_body.sprite.value().getPosition());
                        break;
                    default:
                        // missiles are destroyed
                        ship_o.special.reset();
                        break;
                }
            }
        }

        js::GameObjects::update_ship(ship_o);

        if (ship_o.cannon.has_value())
        {
            auto & c = ship_o.cannon.value();
            if (c.sprite.value().getPosition().x < EDGE_LEFT + c.half_size.x ||
                c.sprite.value().getPosition().x > EDGE_RIGHT - c.half_size.x ||
                c.sprite.value().getPosition().y < EDGE_TOP + c.half_size.y ||
                c.sprite.value().getPosition().y > EDGE_BOTTOM - c.half_size.y)
            {
                ship_o.cannon.reset();
            }
        }

        if (ship_o.special.has_value())
        {
            auto & spc = ship_o.special.value();
            if (spc.sprite.value().getPosition().x < EDGE_LEFT + spc.half_size.x ||
                spc.sprite.value().getPosition().x > EDGE_RIGHT - spc.half_size.x)
            {
                switch (ship_o.special_type)
                {
                    case js::GameObjects::BALL:
                    case js::GameObjects::STAR:
                        // ball and start bounce of screen bounds
                        ship_o.special_velocity.x *= -1;
                        break;
                    default:
                        // missiles are destoryed
                        ship_o.special.reset();
                }
            }
            else 
            if (spc.sprite.value().getPosition().y < EDGE_TOP + spc.half_size.y ||
                spc.sprite.value().getPosition().y > EDGE_BOTTOM - spc.half_size.y)
            {
                switch (ship_o.special_type)
                {
                    case js::GameObjects::BALL:
                    case js::GameObjects::STAR:
                        // ball and start bounce of screen bounds
                        ship_o.special_velocity.y *= -1;
                        break;
                    default:
                        // missiles are destoryed
                        ship_o.special.reset();
                }
            }
        }

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
        if (ship_o.cannon.has_value())
        {
            window.draw(ship_o.cannon.value().sprite.value());
        }
        if (ship_o.special.has_value())
        {
            if (ship_o.special_lifespan > 8)
            {
                ship_o.special.value().sprite.value().setColor(sf::Color(std::rand()%200+55, std::rand()%200+55, std::rand()%200+55));
            }
            window.draw(ship_o.special.value().sprite.value());
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
