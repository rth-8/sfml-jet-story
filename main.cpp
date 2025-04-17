#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>
#include <optional>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

#include "zx_colors.h"
#include "assets.h"
#include "maze.h"
#include "animation.h"
#include "ship.h"
#include "enemies.h"
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
    load_item_textures(assets);
    load_ship_textures(assets);
    load_enemy_textures(assets);
    
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

        for (auto & enemy_o : room_o.enemies)
        {
            js::GameObjects::move_enemy(enemy_o, ship_o, dtAsSeconds, game_frame);
        }

        // collisions

        for (auto & wall_o : room_o.walls)
        {
            sf::Vector2f vec(0.0f, 0.0f);

            // ship vs. walls
            if (checkCollision(ship_o.ship_body.sprite.value().getPosition(), ship_o.ship_body.half_size, ship_o.previous_position, 
                                wall_o.sprite.value().getPosition(), wall_o.half_size, 
                                vec))
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
                if (checkCollision(ship_o.cannon.value(), wall_o))
                {
                    ship_o.cannon.reset();
                }
            }

            // special vs. walls
            if (ship_o.special.has_value())
            {
                vec.x = 0;
                vec.y = 0;
                if (checkCollision(ship_o.special.value().sprite.value().getPosition(), ship_o.special.value().half_size, ship_o.special_previous_position, 
                                    wall_o.sprite.value().getPosition(), wall_o.half_size, 
                                    vec))
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
                            js::GameObjects::reset_special(ship_o);
                            break;
                    }
                }
            }

            // enemies vs. walls
            for (auto & enemy_o : room_o.enemies)
            {
                vec.x = 0;
                vec.y = 0;
                auto e_pos = js::GameObjects::enemy_get_position(enemy_o);
                auto e_hs = js::GameObjects::enemy_get_half_size(enemy_o);
                if (checkCollision(e_pos, e_hs, enemy_o.previous_position, 
                                    wall_o.sprite.value().getPosition(), wall_o.half_size, 
                                    vec))
                {
                    enemy_o.anim.sprite.value().move(vec);
                    if (enemy_o.carried_enemy.has_value())
                    {
                        enemy_o.carried_enemy.value().sprite.value().move(vec);
                    }
                    else
                    if (enemy_o.carried_item.has_value())
                    {
                        enemy_o.carried_item.value().sprite.value().move(vec);
                    }

                    if (enemy_o.anim.id == 15 || enemy_o.anim.id == 16)
                    {
                        if (vec.x != 0)
                        {
                            enemy_o.velocity.x *= -1;
                        }
                        if (vec.y != 0)
                        {
                            enemy_o.velocity.y *= -1;
                        }
                    }
                }

                // screen bounds
                if (enemy_o.anim.sprite.value().getPosition().x < EDGE_LEFT + enemy_o.anim.half_size.x)
                {
                    enemy_o.anim.sprite.value().setPosition({EDGE_LEFT + enemy_o.anim.half_size.x, enemy_o.anim.sprite.value().getPosition().y});
                    enemy_o.velocity.x *= -1;
                }
                if (enemy_o.anim.sprite.value().getPosition().x > EDGE_RIGHT - enemy_o.anim.half_size.x)
                {
                    enemy_o.anim.sprite.value().setPosition({EDGE_RIGHT - enemy_o.anim.half_size.x, enemy_o.anim.sprite.value().getPosition().y});
                    enemy_o.velocity.x *= -1;
                } 
                if (enemy_o.anim.sprite.value().getPosition().y < EDGE_TOP + enemy_o.anim.half_size.y)
                {
                    enemy_o.anim.sprite.value().setPosition({enemy_o.anim.sprite.value().getPosition().x, EDGE_TOP + enemy_o.anim.half_size.y});
                    enemy_o.velocity.y *= -1;
                }
                if (enemy_o.anim.sprite.value().getPosition().y > EDGE_BOTTOM - enemy_o.anim.half_size.y)
                {
                    enemy_o.anim.sprite.value().setPosition({enemy_o.anim.sprite.value().getPosition().x, EDGE_BOTTOM - enemy_o.anim.half_size.y});
                    enemy_o.velocity.y *= -1;
                }
            }
        } // walls collisions

        for (auto & item_o : room_o.items)
        {
            // ship vs item
            if (checkCollision(ship_o.ship_body, item_o))
            {
                int id = item_o.id;
                if (item_o.id == 7) 
                    id = std::rand() % 7;
                
                if (ship_o.special.has_value() && (id == 1 || id == 3 || id == 4 || id == 6))
                {
                    ship_o.next_special = id;
                }
                else
                {
                    js::GameObjects::get_item(ship_o, id);
                }
                item_o.isAlive = false;
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
                        js::GameObjects::reset_special(ship_o);
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
                        js::GameObjects::reset_special(ship_o);
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
                        js::GameObjects::reset_special(ship_o);
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
                        js::GameObjects::reset_special(ship_o);
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
                        // missiles are destroyed
                        js::GameObjects::reset_special(ship_o);
                        break;
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
                        // missiles are destroyed
                        js::GameObjects::reset_special(ship_o);
                        break;
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

        for (auto & enemy_o : room_o.enemies)
        {
            if (enemy_o.anim.sprite.has_value())
            {
                js::GameObjects::animation_update(enemy_o.anim, game_frame);
                if (enemy_o.carried_enemy.has_value())
                {
                    js::GameObjects::animation_update(enemy_o.carried_enemy.value(), game_frame);
                }
            }
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

        for (auto & item_o : room_o.items)
        {
            window.draw(item_o.sprite.value());
            js::GameObjects::update_item(item_o, game_frame);
        }

        for (auto & enemy_o : room_o.enemies)
        {
            if (enemy_o.anim.sprite.has_value())
            {
                window.draw(enemy_o.anim.sprite.value());
                if (enemy_o.carried_enemy.has_value())
                {
                    window.draw(enemy_o.carried_enemy.value().sprite.value());
                }
                if (enemy_o.carried_item.has_value())
                {
                    window.draw(enemy_o.carried_item.value().sprite.value());
                    js::GameObjects::update_item(enemy_o.carried_item.value(), game_frame);
                }
            }

            auto pos = js::GameObjects::enemy_get_position(enemy_o);
            auto size = js::GameObjects::enemy_get_size(enemy_o);
            auto hs = js::GameObjects::enemy_get_half_size(enemy_o);
            drawBB(window, pos, size, hs, sf::Color::Green);
        }

        for (auto & wall_o : room_o.walls)
        {
            window.draw(wall_o.sprite.value());
        }

        window.display();

        // cleanup

        for (std::vector<js::GameObjects::Animation>::iterator it = room_o.items.begin(); it != room_o.items.end();)
        {
            if ((*it).isAlive == false) 
                it = room_o.items.erase(it);
            else 
                ++it;
        }

        game_frame++;
    }

    return 0;
}
