#include "enemies.h"

#include <iostream>
#include <set>

#include "common.h"
#include "physics.h"
#include "zx_colors.h"
#include "assets.h"
#include "maze_data.h"
#include "maze.h"
#include "ship.h"
#include "projectiles.h"
#include "explosions.h"
#include "sounds.h"

#define ENEMY_CARRIED_GAP 2

const std::set<int> non_animated{0, 1, 2, 3, 4, 5, 6, 8, 11, 18};

sf::Vector2f enemy_get_size(Enemy & enemy)
{
    if (enemy.carried_enemy.has_value())
    {
        return { 
            enemy.anim.size.x,
            enemy.anim.size.y + enemy.carried_enemy.value().size.y + ENEMY_CARRIED_GAP
            };
    }
    else
    if (enemy.carried_item.has_value())
    {
        return { 
            enemy.anim.size.x,
            enemy.anim.size.y + enemy.carried_item.value().size.y + ENEMY_CARRIED_GAP
            };
    }
    else
    {
        return sf::Vector2f(enemy.anim.size);
    }
}

sf::Vector2f enemy_get_half_size(Enemy & enemy)
{
    auto size = enemy_get_size(enemy);
    return { size.x * 0.5f, size.y * 0.5f };
}

sf::Vector2f enemy_get_position(Enemy & enemy)
{
    if (enemy.carried_enemy.has_value())
    {
        float h = enemy.anim.sprite.value().getPosition().y - enemy.carried_enemy.value().sprite.value().getPosition().y;
        return { 
            enemy.anim.sprite.value().getPosition().x, 
            enemy.anim.sprite.value().getPosition().y - h*0.5f
            };
    }
    else
    if (enemy.carried_item.has_value())
    {
        float h = enemy.anim.sprite.value().getPosition().y - enemy.carried_item.value().sprite.value().getPosition().y;
        return { 
            enemy.anim.sprite.value().getPosition().x, 
            enemy.anim.sprite.value().getPosition().y - h*0.5f
            };
    }
    else
    {
        return sf::Vector2f(enemy.anim.sprite.value().getPosition());
    }
}

void create_enemy_anim(Animation & anim, const int & id, const int & subid, const Assets & assets)
{
    if (non_animated.contains(id))
    {
        create_animation(anim, id, assets.enemies[id], assets.enemies[id].getSize().x, assets.enemies[id].getSize().y);
    }
    else
    {
        switch (id)
        {
            case 7:  create_animation(anim, id, assets.enemies[id], 50, 45, 4, 10); break;
            case 9:  create_animation(anim, id, assets.enemies[id], 49, 50, 8, 10); break;
            case 10: create_animation(anim, id, assets.enemies[id], 50, 50, 2, 0); break;   // spawner
            case 12: create_animation(anim, id, assets.enemies[id], 50, 50, 4, 6); break;
            case 13: create_animation(anim, id, assets.enemies[id], 47, 50, 2, 6); break;
            case 14: create_animation(anim, id, assets.enemies[id], 50, 50, 2, 6); break;
            case 15: create_animation(anim, id, assets.enemies[id], 50, 50, 4, 6); break;
            case 16: create_animation(anim, id, assets.enemies[id], 50, 50, 4, 6); break;
            case 17: create_animation(anim, id, assets.enemies[id], 50, 47, 2, 6); break;
            case 19: create_animation(anim, id, assets.enemies[id], 94, 50, 2, 6); break;
            case 20: create_animation(anim, id, assets.enemies[id + subid], 50, 50, 2, 6); break;
        }
    }
    anim.subid = subid;
}

/*
0   base                    not moving
1   dish                    not moving      shot - horizontal
2   rod                     not moving      shot - tracking
3   tank                    mirror          shot - horizontal
4   barrier                 not moving
5   missile launcher (up)   not moving      shot - vertical up
6   bomb launcher (down)    not moving      shot - vertical down
7   tesla                   not moving      shot - tracking
8   missile                 not moving      shot - diagonal
9   missile launcher        not moving      shot - horizontal
10  spawner                 not moving
11  drone                   moving
12  bolt drone              moving
13  tank drone              moving          shot - horizontal
14  fighter                 moving
15  rotating drone          moving
16  small ball              moving
17  drone                   moving
18  big ball                moving
19  hoovercraft             moving
20  carrier                 moving
21  carrier                 moving
*/

void create_enemy(Enemy & eo, const EnemyData & ed, const MazeData & md, const Assets & assets)
{
    create_enemy_anim(eo.anim, ed.id, ed.subid, assets);

    if (eo.anim.sprite.has_value())
    {
        eo.anim.sprite.value().setPosition(
            {static_cast<float>(ed.x) + eo.anim.half_size.x, 
             static_cast<float>(ed.y) + eo.anim.half_size.y + 100});

        if ((ed.id == 1 || ed.id == 8) && ed.subid == 0)
        {
            eo.anim.sprite.value().setScale({-1.0f, 1.0f});
        }

        if (ed.id == 15 || ed.id == 16)
        {
            eo.velocity = {200.0f, 200.0f};
        }

        // random zx color from bright range (8-15), but skip black (8)
        eo.anim.color_index = std::rand()%7+9;
        eo.anim.sprite.value().setColor(zx_colors[eo.anim.color_index]);
    }

    if (ed.carried_enemy.has_value())
    {
        eo.carried_enemy = std::make_optional<Animation>();
        create_enemy_anim(eo.carried_enemy.value(), ed.carried_enemy.value().id, ed.carried_enemy.value().subid, assets);

        eo.carried_enemy.value().sprite.value().setPosition(
            {static_cast<float>(ed.carried_enemy.value().x) + eo.carried_enemy.value().half_size.x, 
             static_cast<float>(ed.carried_enemy.value().y) + eo.carried_enemy.value().half_size.y + 100});

        if ((ed.carried_enemy.value().id == 1 || ed.carried_enemy.value().id == 8) && 
            ed.carried_enemy.value().subid == 0)
        {
            eo.carried_enemy.value().sprite.value().setScale({-1.0f, 1.0f});
        }

        eo.carried_enemy.value().color_index = std::rand()%7+9;
        eo.carried_enemy.value().sprite.value().setColor(zx_colors[eo.carried_enemy.value().color_index]);
    }

    if (ed.carried_item.has_value())
    {
        eo.carried_item = std::make_optional<Animation>();
        const sf::Texture & tex = assets.items[ed.carried_item.value().id];
        create_animation(eo.carried_item.value(), ed.carried_item.value().id, tex, tex.getSize().x, tex.getSize().y);
        eo.carried_item.value().sprite.value().setPosition(
            {static_cast<float>(ed.carried_item.value().x) + eo.carried_item.value().half_size.x, 
             static_cast<float>(ed.carried_item.value().y) + eo.carried_item.value().half_size.y + 100 + 6});
        eo.carried_item.value().color_index = 0;
    }

    eo.health = md.enemy_specs[eo.anim.id].health;
    eo.score = md.enemy_specs[eo.anim.id].score;
    eo.shooting_delay = md.enemy_specs[eo.anim.id].shooting_delay;
    eo.shooting_speed = md.enemy_specs[eo.anim.id].shooting_speed;
    eo.shooting_counter = 0;

    if (ed.carried_enemy.has_value())
    {
        eo.carried_enemy_health = md.enemy_specs[ed.carried_enemy.value().id].health;
        eo.carried_enemy_score = md.enemy_specs[ed.carried_enemy.value().id].score;
        eo.shooting_delay = md.enemy_specs[ed.carried_enemy.value().id].shooting_delay;
        eo.shooting_speed = md.enemy_specs[ed.carried_enemy.value().id].shooting_speed;
        eo.shooting_counter = 0;
    }
}

void shooting_horizontal(Enemy & enemy, Animation & enemyAnim, Projectiles & projectiles, Explosions & explosions, Sounds & sounds, 
    const Ship & ship, const Assets & assets, float dt)
{
    if (ship.ship_body.sprite.value().getPosition().y > enemyAnim.sprite.value().getPosition().y - 25.0 &&
        ship.ship_body.sprite.value().getPosition().y < enemyAnim.sprite.value().getPosition().y + 25.0)
    {
        if (enemy.shooting_counter <= 0)
        {
            if (enemyAnim.id == 1)
            {
                auto & prj = create_projectile(projectiles, sounds, assets, enemyAnim.id);
                if (enemyAnim.sprite.value().getScale().x < 0)
                {
                    prj.anim.sprite.value().setPosition({
                        enemyAnim.sprite.value().getPosition().x - enemyAnim.half_size.x,
                        enemyAnim.sprite.value().getPosition().y});
                    prj.velocity = {-300.0f, 0.0f};
                }
                else
                {
                    prj.anim.sprite.value().setPosition({
                        enemyAnim.sprite.value().getPosition().x + enemyAnim.half_size.x,
                        enemyAnim.sprite.value().getPosition().y});
                    prj.velocity = {300.0f, 0.0f};
                }
            }
            else
            if (enemyAnim.id == 3 || enemyAnim.id == 13)
            {
                auto & prj = create_projectile(projectiles, sounds, assets, enemyAnim.id);
                if (enemyAnim.sprite.value().getScale().x < 0)
                {
                    prj.anim.sprite.value().setPosition({
                        enemyAnim.sprite.value().getPosition().x - enemyAnim.half_size.x,
                        enemyAnim.sprite.value().getPosition().y});
                    prj.velocity = {-300.0f, 0.0f};
                }
                else
                {
                    prj.anim.sprite.value().setPosition({
                        enemyAnim.sprite.value().getPosition().x + enemyAnim.half_size.x,
                        enemyAnim.sprite.value().getPosition().y});
                    prj.velocity = {300.0f, 0.0f};
                }
            }
            else
            if (enemyAnim.id == 9 && std::rand()%10 == 0)
            {
                auto & prj = create_projectile(projectiles, sounds, assets, enemyAnim.id);
                prj.anim.sprite.value().setPosition(enemyAnim.sprite.value().getPosition());
                if (ship.ship_body.sprite.value().getPosition().x < enemyAnim.sprite.value().getPosition().x)
                {
                    prj.anim.sprite.value().setScale({-1.0, 1.0});
                    prj.velocity = {-150.0f, 0.0f};
                }
                else
                {
                    prj.anim.sprite.value().setScale({1.0, 1.0});
                    prj.velocity = {150.0f, 0.0f};
                }
                enemyAnim.isAlive = false;
                create_explosion(explosions, enemyAnim.sprite.value().getPosition(), assets);
            }

            enemy.shooting_counter = enemy.shooting_delay + (std::rand() % 20 - 10);
        }
        else
        {
            enemy.shooting_counter -= enemy.shooting_speed * dt;
        }
    }
    else
    {
        enemy.shooting_counter = 0;
    }
}

void shooting_vertical(Enemy & enemy, Animation & enemyAnim, Projectiles & projectiles, Sounds & sounds, 
    const Ship & ship, const Assets & assets, float dt)
{
    if (ship.ship_body.sprite.value().getPosition().x > enemyAnim.sprite.value().getPosition().x - 25.0 &&
        ship.ship_body.sprite.value().getPosition().x < enemyAnim.sprite.value().getPosition().x + 25.0)
    {
        if (enemy.shooting_counter <= 0)
        {
            if (enemyAnim.id == 5)
            {
                auto & prj = create_projectile(projectiles, sounds, assets, enemyAnim.id);
                prj.anim.sprite.value().setPosition({
                    enemyAnim.sprite.value().getPosition().x,
                    enemyAnim.sprite.value().getPosition().y - enemyAnim.size.y});
                prj.velocity = {0.0f, -220.0f};
            }
            else
            if (enemyAnim.id == 6)
            {
                auto & prj = create_projectile(projectiles, sounds, assets, enemyAnim.id);
                prj.anim.sprite.value().setPosition({
                    enemyAnim.sprite.value().getPosition().x,
                    enemyAnim.sprite.value().getPosition().y + enemyAnim.size.y});
                prj.velocity = {0.0f, 150.0f};
            }

            enemy.shooting_counter = enemy.shooting_delay + (std::rand() % 20 - 10);
        }
        else
        {
            enemy.shooting_counter -= enemy.shooting_speed * dt;
        }
    }
    else
    {
        enemy.shooting_counter = 0;
    }
}

void shooting_diagonal(Animation & enemyAnim, Projectiles & projectiles, Explosions & explosions, Sounds & sounds, 
    const Ship & ship, const Assets & assets, float dt)
{
    if (enemyAnim.id == 8 && std::rand()%10 == 0)
    {
        float dx = abs(ship.ship_body.sprite.value().getPosition().x - enemyAnim.sprite.value().getPosition().x);
        float dy = abs(ship.ship_body.sprite.value().getPosition().y - enemyAnim.sprite.value().getPosition().y);
        float d = abs(dx - dy);

        if (d < 12.5)
        {
            if (enemyAnim.sprite.value().getScale().x < 0 && 
                ship.ship_body.sprite.value().getPosition().x < enemyAnim.sprite.value().getPosition().x)
            {
                auto & prj = create_projectile(projectiles, sounds, assets, enemyAnim.id);
                prj.anim.sprite.value().setPosition(enemyAnim.sprite.value().getPosition());
                prj.anim.sprite.value().setScale({-1.0, 1.0});
                prj.velocity = {-200.0f, -200.0f};
                enemyAnim.isAlive = false;
                create_explosion(explosions, enemyAnim.sprite.value().getPosition(), assets);
            }
            else
            if (enemyAnim.sprite.value().getScale().x > 0 && 
                ship.ship_body.sprite.value().getPosition().x > enemyAnim.sprite.value().getPosition().x)
            {
                auto & prj = create_projectile(projectiles, sounds, assets, enemyAnim.id);
                prj.anim.sprite.value().setPosition(enemyAnim.sprite.value().getPosition());
                prj.anim.sprite.value().setScale({1.0, 1.0});
                prj.velocity = {200.0f, -200.0f};
                enemyAnim.isAlive = false;
                create_explosion(explosions, enemyAnim.sprite.value().getPosition(), assets);
            }
        }
    }
}

void shooting_tracking(Enemy & enemy, Animation & enemyAnim, Projectiles & projectiles, Sounds & sounds, 
    const Ship & ship, const Assets & assets, float dt)
{
    if (enemy.shooting_counter <= 0)
    {
        auto & prj = create_projectile(projectiles, sounds, assets, enemyAnim.id);
        prj.anim.sprite.value().setPosition(enemyAnim.sprite.value().getPosition());
        prj.velocity = ship.ship_body.sprite.value().getPosition() - enemyAnim.sprite.value().getPosition();
        prj.velocity = prj.velocity.normalized() * 200.0f;

        enemy.shooting_counter = enemy.shooting_delay + (std::rand() % 20 - 10);
    }
    else
    {
        enemy.shooting_counter -= enemy.shooting_speed * dt;
    }
}

void move_enemy_mirror(Enemy & enemy, const Ship & ship)
{
    if (ship.ship_body.sprite.value().getPosition().x < enemy.anim.sprite.value().getPosition().x)
    {
        enemy.anim.sprite.value().setScale({-1.0, 1.0});
    }
    else
    {
        enemy.anim.sprite.value().setScale({1.0, 1.0});
    }
}

void move_enemy_random_vertical_tracking(Enemy & enemy, const Ship & ship, float dt, int gFrame)
{
    if (gFrame % 10 == 0)
    {
        enemy.velocity += {std::rand()%300 - 150.0f, std::rand()%300 - 150.0f};
        enemy.velocity += {0.0f, enemy.anim.sprite.value().getPosition().y - ship.ship_body.sprite.value().getPosition().y};
        enemy.velocity *= dt * -1.0f;
    }
    enemy.anim.sprite.value().move(enemy.velocity);
}

void move_enemy_random(Enemy & enemy, float dt, int gFrame)
{
    if (gFrame % 20 == 0)
    {
        enemy.velocity += {std::rand()%300 - 150.0f, std::rand()%300 - 150.0f};
        enemy.velocity *= dt;
    }
    enemy.anim.sprite.value().move(enemy.velocity);
}

void move_enemy_constant(Enemy & enemy, float dt)
{
    enemy.anim.sprite.value().move(enemy.velocity * dt);
}

void spawn_enemy(Room & room, Enemy & spawner, Animation & spawnerAnim, Sounds & sounds, const MazeData & md, const Assets & assets, float dt)
{
    if (spawner.shooting_counter <= 0)
    {
        Enemy eo;
        int id = std::rand() % (17-11+1) + 11;
        create_enemy_anim(eo.anim, id, 0, assets);
        eo.anim.sprite.value().setPosition(spawnerAnim.sprite.value().getPosition());
        if (id == 15 || id == 16)
        {
            eo.velocity = {200.0f, 200.0f};
        }
        eo.anim.color_index = std::rand()%7+9;
        eo.anim.sprite.value().setColor(zx_colors[eo.anim.color_index]);
        eo.health = md.enemy_specs[id].health;
        eo.score = md.enemy_specs[id].score;
        eo.shooting_delay = md.enemy_specs[id].shooting_delay;
        eo.shooting_speed = md.enemy_specs[id].shooting_speed;

        room.to_be_added.push_back(eo); // new enemy should not be added to room's enemies vector immediately

        sounds.sounds.at(ENEMY_10_SPAWN).play();
        set_frame(spawnerAnim, ((spawnerAnim.frame + 1) % spawnerAnim.frame_count));
        spawner.shooting_counter = spawner.shooting_delay + (std::rand() % 20 - 10);
    }
    else
    {
        spawner.shooting_counter -= spawner.shooting_speed * dt;
    }
}

void move_enemy_carried_element(Room & room, Enemy & enemy, Projectiles & projectiles, Explosions & explosions, Sounds & sounds, 
    const Ship & ship, const MazeData & md, const Assets & assets, float dt)
{
    if (enemy.carried_enemy.has_value())
    {
        switch (enemy.carried_enemy.value().id)
        {
            case 1:  shooting_horizontal(enemy, enemy.carried_enemy.value(), projectiles, explosions, sounds, ship, assets, dt); break;
            case 2:  shooting_tracking(enemy, enemy.carried_enemy.value(), projectiles, sounds, ship, assets, dt); break;
            case 3:  shooting_horizontal(enemy, enemy.carried_enemy.value(), projectiles, explosions, sounds, ship, assets, dt); break;
            case 5:  shooting_vertical(enemy, enemy.carried_enemy.value(), projectiles, sounds, ship, assets, dt); break;
            case 7:  shooting_tracking(enemy, enemy.carried_enemy.value(), projectiles, sounds, ship, assets, dt); break;
            case 8:  shooting_diagonal(enemy.carried_enemy.value(), projectiles, explosions, sounds, ship, assets, dt); break;
            case 9:  shooting_horizontal(enemy, enemy.carried_enemy.value(), projectiles, explosions, sounds, ship, assets, dt); break;
            case 10: spawn_enemy(room, enemy, enemy.carried_enemy.value(), sounds, md, assets, dt); break;
        }
        if (enemy.carried_enemy.value().isAlive == false)
        {
            enemy.carried_enemy.reset();
            return;
        }

        enemy.carried_enemy.value().sprite.value().setPosition({
            enemy.anim.sprite.value().getPosition().x,
            enemy.anim.sprite.value().getPosition().y - enemy.anim.half_size.y - enemy.carried_enemy.value().half_size.y - ENEMY_CARRIED_GAP
        });

        if (enemy.carried_enemy.value().id == 3)
        {
            if (ship.ship_body.sprite.value().getPosition().x < enemy.carried_enemy.value().sprite.value().getPosition().x)
            {
                enemy.carried_enemy.value().sprite.value().setScale({-1.0, 1.0});
            }
            else
            {
                enemy.carried_enemy.value().sprite.value().setScale({1.0, 1.0});
            }
        }
    }
    else
    if (enemy.carried_item.has_value())
    {
        enemy.carried_item.value().sprite.value().setPosition({
            enemy.anim.sprite.value().getPosition().x,
            enemy.anim.sprite.value().getPosition().y - enemy.anim.half_size.y - enemy.carried_item.value().half_size.y - ENEMY_CARRIED_GAP
        });
    }
}

void move_enemy(Room & room, Enemy & enemy, Projectiles & projectiles, Explosions & explosions, Sounds & sounds, 
    const Ship & ship, const MazeData & md, const Assets & assets, float dt, int gFrame)
{
    enemy.previous_position = enemy_get_position(enemy);

    switch (enemy.anim.id)
    {
        case 1:
            shooting_horizontal(enemy, enemy.anim, projectiles, explosions, sounds, ship, assets, dt);
            break;
        case 2:
            shooting_tracking(enemy, enemy.anim, projectiles, sounds, ship, assets, dt);
            break;
        case 3:
            move_enemy_mirror(enemy, ship);
            shooting_horizontal(enemy, enemy.anim, projectiles, explosions, sounds, ship, assets, dt);
            break;
        case 5:
        case 6:
            shooting_vertical(enemy, enemy.anim, projectiles, sounds, ship, assets, dt);
            break;
        case 7:
            shooting_tracking(enemy, enemy.anim, projectiles, sounds, ship, assets, dt);
            break;
        case 8:
            shooting_diagonal(enemy.anim, projectiles, explosions, sounds, ship, assets, dt);
            break;
        case 9:
            shooting_horizontal(enemy, enemy.anim, projectiles, explosions, sounds, ship, assets, dt);
            break;
        case 10:
            spawn_enemy(room, enemy, enemy.anim, sounds, md, assets, dt);
            break;
        case 11:
            move_enemy_mirror(enemy, ship);
            move_enemy_random_vertical_tracking(enemy, ship, dt, gFrame); 
            break;
        case 12: 
            move_enemy_random(enemy, dt, gFrame); 
            break;
        case 13: 
            move_enemy_mirror(enemy, ship);
            move_enemy_random_vertical_tracking(enemy, ship, dt, gFrame);
            shooting_horizontal(enemy, enemy.anim, projectiles, explosions, sounds, ship, assets, dt);
            break;
        case 14: 
            move_enemy_mirror(enemy, ship);
            move_enemy_random_vertical_tracking(enemy, ship, dt, gFrame); 
            break;
        case 15:
        case 16:
            move_enemy_constant(enemy, dt); 
            break;
        case 17:
            move_enemy_mirror(enemy, ship);
            move_enemy_random_vertical_tracking(enemy, ship, dt, gFrame);
            break;
        case 18:
        case 19:
            move_enemy_random(enemy, dt, gFrame);
            break;
        case 20:
            move_enemy_random(enemy, dt, gFrame);
            move_enemy_carried_element(room, enemy, projectiles, explosions, sounds, ship, md, assets, dt);
            break;
    }
}

void collision_enemy_wall(Enemy & enemy, Animation & wall)
{
    sf::Vector2f vec(0.0f, 0.0f);

    auto e_pos = enemy_get_position(enemy);
    auto e_hs = enemy_get_half_size(enemy);

    if (checkCollision(e_pos, e_hs, enemy.previous_position, 
                        wall.sprite.value().getPosition(), wall.half_size, 
                        vec))
    {
        enemy.anim.sprite.value().move(vec);
        if (enemy.carried_enemy.has_value())
        {
            enemy.carried_enemy.value().sprite.value().move(vec);
        }
        else
        if (enemy.carried_item.has_value())
        {
            enemy.carried_item.value().sprite.value().move(vec);
        }

        if (enemy.anim.id == 15 || enemy.anim.id == 16)
        {
            if (vec.x != 0)
            {
                enemy.velocity.x *= -1;
            }
            if (vec.y != 0)
            {
                enemy.velocity.y *= -1;
            }
        }
    }
}

bool collision_enemy_ship(Enemy & enemy, Maze & maze, Ship & ship, Explosions & explosions, Sounds & sounds, const Assets & assets)
{
    bool result = false;

    if (checkCollision(enemy.anim, ship.ship_body))
    {
        if (ship.damage_delay == 0)
        {
            ship.shield -= 10;
            enemy.health -= 10;
            sounds.sounds.at(DAMAGE).play();
            if (enemy.health <= 0)
            {
                enemy.anim.isAlive = false;
                maze.score += enemy.score;
                sounds.sounds.at(DAMAGE).stop();
                if (enemy.anim.id == 0)
                {
                    maze.base_cnt--;
                    sounds.sounds.at(BOOM_BASE).play();
                    explosions.flashing_counter = FLASH_MAX;
                }
                else
                {
                    sounds.sounds.at(BOOM).play();
                }
                create_explosion(explosions, enemy.anim.sprite.value().getPosition(), assets);
            }
            else
            {
                ship.damage_delay = 10;
            }
        }
        else
        {
            ship.damage_delay--;
        }

        result = true;
    }

    if (enemy.anim.isAlive == true && enemy.carried_enemy.has_value())
    {
        if (checkCollision(enemy.carried_enemy.value(), ship.ship_body))
        {
            if (ship.damage_delay == 0)
            {
                ship.shield -= 10;
                enemy.carried_enemy_health -= 10;
                sounds.sounds.at(DAMAGE).play();
                if (enemy.carried_enemy_health <= 0)
                {
                    maze.score += enemy.carried_enemy_score;
                    sounds.sounds.at(DAMAGE).stop();
                    if (enemy.carried_enemy.value().id == 0)
                    {
                        maze.base_cnt--;
                        sounds.sounds.at(BOOM_BASE).play();
                        explosions.flashing_counter = FLASH_MAX;
                    }
                    else
                    {
                        sounds.sounds.at(BOOM).play();
                    }
                    create_explosion(explosions, enemy.carried_enemy.value().sprite.value().getPosition(), assets);
                    enemy.carried_enemy.reset();
                }
                else
                {
                    ship.damage_delay = 10;
                }
            }
            else
            {
                ship.damage_delay--;
            }

            result = true;
        }
    }

    return result;
}

void collision_enemy_cannon(Enemy & enemy, Maze & maze, Ship & ship, Explosions & explosions, Sounds & sounds, const Assets & assets)
{
    if (checkCollision(enemy.anim, ship.cannon.value()))
    {
        enemy.health -= 10;
        ship.cannon.reset();
        sounds.sounds.at(HIT_ENEMY).play();
        if (enemy.health <= 0)
        {
            enemy.anim.isAlive = false;
            maze.score += enemy.score;
            if (enemy.anim.id == 0)
            {
                maze.base_cnt--;
                sounds.sounds.at(BOOM_BASE).play();
                explosions.flashing_counter = FLASH_MAX;
            }
            else
            {
                sounds.sounds.at(BOOM).play();
            }
            create_explosion(explosions, enemy.anim.sprite.value().getPosition(), assets);
        }
    }

    if (enemy.anim.isAlive == true && enemy.carried_enemy.has_value())
    {
        if (checkCollision(enemy.carried_enemy.value(), ship.cannon.value()))
        {
            enemy.carried_enemy_health -= 10;
            ship.cannon.reset();
            sounds.sounds.at(HIT_ENEMY).play();
            if (enemy.carried_enemy_health <= 0)
            {
                maze.score += enemy.carried_enemy_score;
                if (enemy.carried_enemy.value().id == 0)
                {
                    maze.base_cnt--;
                    sounds.sounds.at(BOOM_BASE).play();
                    explosions.flashing_counter = FLASH_MAX;
                }
                else
                {
                    sounds.sounds.at(BOOM).play();
                }
                create_explosion(explosions, enemy.carried_enemy.value().sprite.value().getPosition(), assets);
                enemy.carried_enemy.reset();
            }
        }
    }
}

void collision_enemy_special(Enemy & enemy, Maze & maze, Ship & ship, Explosions & explosions, Sounds & sounds, const Assets & assets)
{
    if (checkCollision(enemy.anim, ship.special.value()))
    {
        enemy.health = 0;
        if (ship.special_type == MISSILE_SIDE || ship.special_type == MISSILE_DOWN)
        {
            ship.special.reset();
        }
        enemy.anim.isAlive = false;
        maze.score += enemy.score;
        if (enemy.anim.id == 0)
        {
            maze.base_cnt--;
            sounds.sounds.at(BOOM_BASE).play();
            explosions.flashing_counter = FLASH_MAX;
        }
        else
        {
            sounds.sounds.at(BOOM).play();
        }
        create_explosion(explosions, enemy.anim.sprite.value().getPosition(), assets);
    }

    if (enemy.anim.isAlive == true && enemy.carried_enemy.has_value())
    {
        if (checkCollision(enemy.carried_enemy.value(), ship.special.value()))
        {
            enemy.carried_enemy_health = 0;
            maze.score += enemy.carried_enemy_score;
            if (ship.special_type == MISSILE_SIDE || ship.special_type == MISSILE_DOWN)
            {
                ship.special.reset();
            }
            if (enemy.carried_enemy.value().id == 0)
            {
                maze.base_cnt--;
                sounds.sounds.at(BOOM_BASE);
                explosions.flashing_counter = FLASH_MAX;
            }
            else
            {
                sounds.sounds.at(BOOM).play();
            }
            create_explosion(explosions, enemy.carried_enemy.value().sprite.value().getPosition(), assets);
            enemy.carried_enemy.reset();
        }
    }
}

void enemy_check_bounds(Enemy & enemy)
{
    if (enemy.anim.sprite.value().getPosition().x < EDGE_LEFT + enemy.anim.half_size.x)
    {
        enemy.anim.sprite.value().setPosition({EDGE_LEFT + enemy.anim.half_size.x, enemy.anim.sprite.value().getPosition().y});
        enemy.velocity.x *= -1;
    }
    if (enemy.anim.sprite.value().getPosition().x > EDGE_RIGHT - enemy.anim.half_size.x)
    {
        enemy.anim.sprite.value().setPosition({EDGE_RIGHT - enemy.anim.half_size.x, enemy.anim.sprite.value().getPosition().y});
        enemy.velocity.x *= -1;
    } 
    if (enemy.anim.sprite.value().getPosition().y < EDGE_TOP + enemy.anim.half_size.y)
    {
        enemy.anim.sprite.value().setPosition({enemy.anim.sprite.value().getPosition().x, EDGE_TOP + enemy.anim.half_size.y});
        enemy.velocity.y *= -1;
    }
    if (enemy.anim.sprite.value().getPosition().y > EDGE_BOTTOM - enemy.anim.half_size.y)
    {
        enemy.anim.sprite.value().setPosition({enemy.anim.sprite.value().getPosition().x, EDGE_BOTTOM - enemy.anim.half_size.y});
        enemy.velocity.y *= -1;
    }

    if (enemy.carried_enemy.has_value())
    {
        auto e_pos = enemy_get_position(enemy);
        auto e_hs = enemy_get_half_size(enemy);
        if (e_pos.y < EDGE_TOP + e_hs.y)
        {
            enemy.anim.sprite.value().move({0.0f, abs(e_pos.y - e_hs.y - EDGE_TOP)});
            enemy.carried_enemy.value().sprite.value().move({0.0f, abs(e_pos.y - e_hs.y - EDGE_TOP)});
            enemy.velocity.y *= -1;
        }
    }
    else
    if (enemy.carried_item.has_value())
    {
        auto e_pos = enemy_get_position(enemy);
        auto e_hs = enemy_get_half_size(enemy);
        if (e_pos.y < EDGE_TOP + e_hs.y)
        {
            enemy.anim.sprite.value().move({0.0f, abs(e_pos.y - e_hs.y - EDGE_TOP)});
            enemy.carried_item.value().sprite.value().move({0.0f, abs(e_pos.y - e_hs.y - EDGE_TOP)});
            enemy.velocity.y *= -1;
        }
    }
}
