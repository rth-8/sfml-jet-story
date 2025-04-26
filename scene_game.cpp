#include "scene_game.h"
#include "common.h"
#include "zx_colors.h"
#include "assets.h"
#include "sounds.h"
#include "maze_data.h"
#include "maze.h"
#include "ship.h"
#include "projectiles.h"
#include "explosions.h"
#include "infobar.h"
#include "physics.h"

void scene_game_input(Ship & ship, Sounds & sounds, const Assets & assets, float dt)
{
    ship.previous_position = ship.ship_body.sprite.value().getPosition();
    if (ship.cannon.has_value())
    {
        ship.cannon_previous_position = ship.cannon.value().sprite.value().getPosition();
    }
    if (ship.special.has_value()) 
    {
        ship.special_previous_position = ship.special.value().sprite.value().getPosition();
    }

    ship.thrust_up = false;
    ship.thrust_horiz = false;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
    {
        ship.velocity.y -= ACCEL_VERT * dt;
        ship.fuel -= FUEL_SUB * dt;
        ship.thrust_up = true;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
    {
        // launch special
        if (!ship.special.has_value() && ship.special_ammo > 0)
        {
            ship.special_ammo--;
            create_special(ship, assets);
            sounds.sounds.at(SPECIAL_LAUNCH).play();
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
    {
        ship.velocity.x -= ACCEL_HORIZ * dt;
        ship.fuel -= FUEL_SUB * dt;
        ship.ship_body.sprite.value().setScale({-1, 1});
        ship.thrust_horiz = true;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
    {
        ship.velocity.x += ACCEL_HORIZ * dt;
        ship.fuel -= FUEL_SUB * dt;
        ship.ship_body.sprite.value().setScale({1, 1});
        ship.thrust_horiz = true;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
    {
        if (!ship.cannon.has_value() && ship.cannon_ammo > 0)
        {
            ship.cannon_ammo--;
            create_cannon(ship, assets);
            sounds.sounds.at(CANNON_SHOT).play();
        }
    }
}

void scene_game_update(Ship & ship, Maze & maze, Projectiles & projectiles, Explosions & explosions, Sounds & sounds, 
    const MazeData & mazeData, const Assets & assets, float dt, int gFrame)
{
    auto & room = get_current_room(maze);

    move_ship(dt, ship);
    if (ship.cannon.has_value())
    {
        move_cannon(dt, ship);
    }
    if (ship.special.has_value())
    {
        move_special(dt, ship);
    }

    for (auto & enemy : room.enemies)
    {
        move_enemy(room, enemy, projectiles, explosions, sounds, ship, mazeData, assets, dt, gFrame);
    }

    for (auto & prj : projectiles.projectiles)
    {
        move_projectile(prj, sounds, dt);
    }

    for (auto & fg : explosions.fragments)
    {
        move_fragment(fg, dt);
    }
}

bool scene_game_collisions(Ship & ship, Maze & maze, Projectiles & projectiles, Explosions & explosions, Sounds & sounds, 
    const Assets & assets)
{
    auto & room = get_current_room(maze);

    // walls collisions
    for (auto & wall : room.walls)
    {
        collision_ship_wall(ship, wall);
        collision_cannon_wall(ship, wall);
        collision_special_wall(ship, wall, sounds);
        for (auto & enemy : room.enemies)
        {
            collision_enemy_wall(enemy, wall);
        }

        for (auto & prj : projectiles.projectiles)
        {
            collision_projectile_wall(prj, wall, explosions, sounds, assets);
        }

        for (auto & fg : explosions.fragments)
        {
            if (checkCollision(fg.anim, wall))
            {
                fg.anim.isAlive = false;
            }
        }
    }

    // ship vs. items
    for (auto & item : room.items)
    {
        collision_ship_item(ship, item, sounds);
    }

    // enemies collisions
    bool anyCollision = false;
    for (auto & enemy : room.enemies)
    {
        if (collision_enemy_ship(enemy, maze, ship, explosions, sounds, assets))
        {
            anyCollision = true;
        }
        if (ship.cannon.has_value())
        {
            collision_enemy_cannon(enemy, maze, ship, explosions, sounds, assets);
        }
        if (ship.special.has_value())
        {
            collision_enemy_special(enemy, maze, ship, explosions, sounds, assets);
        }
        enemy_check_bounds(enemy);
    }
    if (!anyCollision)
    {
        sounds.sounds.at(DAMAGE).stop();
    }

    // enemy projectiles collisions
    for (auto & prj : projectiles.projectiles)
    {
        collision_projectile_ship(prj, ship, explosions, sounds, assets);
        projectile_check_bounds(prj, sounds);
    }

    // checking bounds

    for (auto & fg : explosions.fragments)
    {
        fragment_check_bounds(fg);
    }

    cannon_check_bounds(ship);
    special_check_bounds(ship, sounds);
    if (ship_check_bounds(maze, ship))
    {
        projectiles.projectiles.clear();
        explosions.explosions.clear();
        explosions.fragments.clear();

        for (int i = DAMAGE; i != LAST; i++)
        {
            SoundTypes t = static_cast<SoundTypes>(i);
            sounds.sounds.at(t).stop();
        }
        
        return true;
    }

    return false;
}

void scene_game_animations(Ship & ship, Maze & maze, Projectiles & projectiles, Explosions & explosions, float dt)
{
    auto & room = get_current_room(maze);

    update_flames_positions(ship);

    if (ship.thrust_horiz)
    {
        animation_update(ship.ship_flame_back);
    }
    if (ship.thrust_up) 
    {
        animation_update(ship.ship_flame_down_big);
        animation_update(ship.ship_flame_down_small);
    }

    for (auto & enemy : room.enemies)
    {
        if (enemy.anim.sprite.has_value())
        {
            animation_update(enemy.anim);
            if (enemy.carried_enemy.has_value())
            {
                animation_update(enemy.carried_enemy.value());
            }
        }
    }

    for (auto & prj : projectiles.projectiles)
    {
        animation_update(prj.anim);
    }

    for (auto & expl : explosions.explosions)
    {
        animation_update(expl);
        if (has_animation_ended(expl))
        {
            expl.isAlive = false;
        }
    }

    for (auto & fg : explosions.fragments)
    {
        if (fg.anim.counter % fg.anim.speed == 0)
        {
            set_frame(fg.anim, std::rand() % fg.anim.frame_count);
        }
        fg.anim.counter++;
    }

    if (explosions.flashing_counter > 0)
    {
        explosions.flashing_counter -= FLASH_SPD * dt;
        if (explosions.flashing_counter <= 0) 
            explosions.flashing_counter = 0;
    }
}

void scene_game_draw(sf::RenderWindow & window, Ship & ship, Maze & maze, Projectiles & projectiles, Explosions & explosions, 
    InfoBar & infobar, int gFrame)
{
    auto & room = get_current_room(maze);

    if (explosions.flashing_counter == 0)
    {
        window.clear();
    }
    else
    if (explosions.flashing_counter < FLASH_MID)
    {
        window.clear(sf::Color::Yellow);
    }
    else
    {
        window.clear(sf::Color::White);
    }

    draw_infobar(window, infobar);

    // draw Ship
    window.draw(ship.ship_body.sprite.value());
    if (ship.thrust_horiz)
    {
        window.draw(ship.ship_flame_back.sprite.value());
    }
    if (ship.thrust_up)
    {
        window.draw(ship.ship_flame_down_big.sprite.value());
        window.draw(ship.ship_flame_down_small.sprite.value());
    }
    if (ship.cannon.has_value())
    {
        window.draw(ship.cannon.value().sprite.value());
    }
    if (ship.special.has_value())
    {
        if (ship.special_lifespan > 8)
        {
            ship.special.value().sprite.value().setColor(zx_colors[std::rand()%7+9]);
        }
        window.draw(ship.special.value().sprite.value());
    }

    // draw items
    for (auto & item : room.items)
    {
        window.draw(item.sprite.value());
        update_item(item, gFrame);
    }

    // draw enemies
    for (auto & enemy : room.enemies)
    {
        if (enemy.anim.sprite.has_value())
        {
            window.draw(enemy.anim.sprite.value());
            if (enemy.carried_enemy.has_value())
            {
                window.draw(enemy.carried_enemy.value().sprite.value());
            }
            if (enemy.carried_item.has_value())
            {
                window.draw(enemy.carried_item.value().sprite.value());
                update_item(enemy.carried_item.value(), gFrame);
            }
        }
    }

    // draw projectiles
    for (auto & prj : projectiles.projectiles)
    {
        window.draw(prj.anim.sprite.value());
    }

    // draw explosions
    for (auto & expl : explosions.explosions)
    {
        window.draw(expl.sprite.value());
    }

    // draw fragments
    for (auto & fg : explosions.fragments)
    {
        window.draw(fg.anim.sprite.value());
    }

    // draw walls
    for (auto & wall : room.walls)
    {
        window.draw(wall.sprite.value());
    }

    window.display();
}

void scene_game_cleanup(Maze & maze, Projectiles & projectiles, Explosions & explosions)
{
    auto & room = get_current_room(maze);

    for (std::vector<Animation>::iterator it = room.items.begin(); it != room.items.end();)
    {
        if ((*it).isAlive == false) 
            it = room.items.erase(it);
        else
            ++it;
    }

    for (std::vector<Enemy>::iterator it = room.enemies.begin(); it != room.enemies.end();)
    {
        if ((*it).anim.isAlive == false) 
            it = room.enemies.erase(it);
        else
            ++it;
    }

    for (std::vector<Projectile>::iterator it = projectiles.projectiles.begin(); it != projectiles.projectiles.end();)
    {
        if ((*it).anim.isAlive == false) 
            it = projectiles.projectiles.erase(it);
        else
            ++it;
    }

    for (std::vector<Animation>::iterator it = explosions.explosions.begin(); it != explosions.explosions.end();)
    {
        if ((*it).isAlive == false) 
            it = explosions.explosions.erase(it);
        else
            ++it;
    }

    for (std::vector<Fragment>::iterator it = explosions.fragments.begin(); it != explosions.fragments.end();)
    {
        if ((*it).anim.isAlive == false) 
            it = explosions.fragments.erase(it);
        else
            ++it;
    }

    // enemies spawned by #10 go to room's enemies vecotr
    std::copy(room.to_be_added.begin(), room.to_be_added.end(), std::back_inserter(room.enemies));
    room.to_be_added.clear();
}
