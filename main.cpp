#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>
#include <optional>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

#define EDGE_TOP 100
#define EDGE_BOTTOM 600
#define EDGE_LEFT 0
#define EDGE_RIGHT 800

#define ACCEL_VERT 20.0f
#define ACCEL_HORIZ 7.0f

#include "zx_colors.h"
#include "assets.h"
#include "maze_data.h"
#include "animation.h"
#include "maze_structures.h"
#include "physics.h"
#include "ship.h"
#include "enemies.h"
#include "maze.h"
#include "debug_draw.h"

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
    
    MazeData maze;
    load_maze(maze);
    
    Maze maze_o;
    create_maze(maze_o, maze, assets);
    
    Ship ship_o;
    create_ship(ship_o, {200,310}, assets);

    while (window.isOpen())
    {
        sf::Time dt = clock.restart();
        float dtAsSeconds = dt.asSeconds();
        // std::cout << "dt = " << dtAsSeconds << "\n";
        
        auto & room_o = get_current_room(maze_o);

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
                create_special(ship_o, assets);
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
                create_cannon(ship_o, assets);
            }
        }

        // update

        move_ship(dtAsSeconds, ship_o);
        if (ship_o.cannon.has_value())
        {
            move_cannon(dtAsSeconds, ship_o);
        }
        if (ship_o.special.has_value())
        {
            move_special(dtAsSeconds, ship_o);
        }

        for (auto & enemy_o : room_o.enemies)
        {
            move_enemy(enemy_o, ship_o, dtAsSeconds, game_frame);
        }

        // collisions

        for (auto & wall_o : room_o.walls)
        {
            collision_ship_wall(ship_o, wall_o);
            collision_cannon_wall(ship_o, wall_o);
            collision_special_wall(ship_o, wall_o);
            for (auto & enemy_o : room_o.enemies)
            {
                collision_enemy_wall(enemy_o, wall_o);
            }
        }

        for (auto & item_o : room_o.items)
        {
            collision_ship_item(ship_o, item_o);
        }

        ship_check_bounds(maze_o, ship_o);
        cannon_check_bounds(ship_o);
        special_check_bounds(ship_o);

        for (auto & enemy_o : room_o.enemies)
        {
            enemy_check_bounds(enemy_o);
        }

        // animations

        update_flames_positions(ship_o);

        if (ship_o.thrust_horiz)
        {
            animation_update(ship_o.ship_flame_back, game_frame);
        }
        if (ship_o.thrust_up) 
        {
            animation_update(ship_o.ship_flame_down_big, game_frame);
            animation_update(ship_o.ship_flame_down_small, game_frame);
        }

        for (auto & enemy_o : room_o.enemies)
        {
            if (enemy_o.anim.sprite.has_value())
            {
                animation_update(enemy_o.anim, game_frame);
                if (enemy_o.carried_enemy.has_value())
                {
                    animation_update(enemy_o.carried_enemy.value(), game_frame);
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
                ship_o.special.value().sprite.value().setColor(zx_colors[std::rand()%7+9]);
            }
            window.draw(ship_o.special.value().sprite.value());
        }

        for (auto & item_o : room_o.items)
        {
            window.draw(item_o.sprite.value());
            update_item(item_o, game_frame);
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
                    update_item(enemy_o.carried_item.value(), game_frame);
                }
            }

            // auto pos = enemy_get_position(enemy_o);
            // auto size = enemy_get_size(enemy_o);
            // auto hs = enemy_get_half_size(enemy_o);
            // drawBB(window, pos, size, hs, sf::Color::Green);
        }

        for (auto & wall_o : room_o.walls)
        {
            window.draw(wall_o.sprite.value());
        }

        window.display();

        // cleanup

        for (std::vector<Animation>::iterator it = room_o.items.begin(); it != room_o.items.end();)
        {
            if ((*it).isAlive == false) 
                it = room_o.items.erase(it);
            else
                ++it;
        }

        for (std::vector<Enemy>::iterator it = room_o.enemies.begin(); it != room_o.enemies.end();)
        {
            if ((*it).anim.isAlive == false) 
                it = room_o.enemies.erase(it);
            else
                ++it;
        }

        game_frame++;
    }

    return 0;
}
