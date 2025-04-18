#ifndef ENEMIES_H
#define ENEMIES_H

#include <set>

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
            case 10: create_animation(anim, id, assets.enemies[id], 50, 50, 2, 500); break;
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
}

void create_enemy(Enemy & eo, const EnemyData & enemy, const Assets & assets)
{
    create_enemy_anim(eo.anim, enemy.id, enemy.subid, assets);

    if (eo.anim.sprite.has_value())
    {
        eo.anim.sprite.value().setPosition(
            {static_cast<float>(enemy.x) + eo.anim.half_size.x, 
             static_cast<float>(enemy.y) + eo.anim.half_size.y + 100});

        if ((enemy.id == 1 || enemy.id == 8) && enemy.subid == 0)
        {
            eo.anim.sprite.value().setScale({-1.0f, 1.0f});
        }

        if (enemy.id == 15 || enemy.id == 16)
        {
            eo.velocity = {200.0f, 200.0f};
        }

        // random zx color from bright range (8-15), but skip black (8)
        eo.anim.color_index = std::rand()%7+9;
        eo.anim.sprite.value().setColor(zx_colors[eo.anim.color_index]);
    }

    if (enemy.carried_enemy.has_value())
    {
        eo.carried_enemy = std::make_optional<Animation>();
        create_enemy_anim(eo.carried_enemy.value(), enemy.carried_enemy.value().id, enemy.carried_enemy.value().subid, assets);

        eo.carried_enemy.value().sprite.value().setPosition(
            {static_cast<float>(enemy.carried_enemy.value().x) + eo.carried_enemy.value().half_size.x, 
             static_cast<float>(enemy.carried_enemy.value().y) + eo.carried_enemy.value().half_size.y + 100});

        if ((enemy.carried_enemy.value().id == 1 || enemy.carried_enemy.value().id == 8) && 
            enemy.carried_enemy.value().subid == 0)
        {
            eo.carried_enemy.value().sprite.value().setScale({-1.0f, 1.0f});
        }

        eo.carried_enemy.value().color_index = std::rand()%7+9;
        eo.carried_enemy.value().sprite.value().setColor(zx_colors[eo.anim.color_index]);
    }

    if (enemy.carried_item.has_value())
    {
        eo.carried_item = std::make_optional<Animation>();
        const sf::Texture & tex = assets.items[enemy.carried_item.value().id];
        create_animation(eo.carried_item.value(), enemy.carried_item.value().id, tex, tex.getSize().x, tex.getSize().y);
        eo.carried_item.value().sprite.value().setPosition(
            {static_cast<float>(enemy.carried_item.value().x) + eo.carried_item.value().half_size.x, 
             static_cast<float>(enemy.carried_item.value().y) + eo.carried_item.value().half_size.y + 100 + 6});
        eo.carried_item.value().color_index = 0;
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

void move_enemy_carried_element(Enemy & enemy, const Ship & ship)
{
    if (enemy.carried_enemy.has_value())
    {
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

void shooting_horizontal(Enemy & enemy, const Ship & ship, float dt, int gFrame)
{

}

void shooting_vertical(Enemy & enemy, const Ship & ship, float dt, int gFrame)
{

}

void shooting_tracking(Enemy & enemy, const Ship & ship, float dt, int gFrame)
{

}

void move_enemy(Enemy & enemy, const Ship & ship, float dt, int gFrame)
{
    enemy.previous_position = enemy_get_position(enemy);

    switch (enemy.anim.id)
    {
        case 1:
            shooting_horizontal(enemy, ship, dt, gFrame); 
            break;
        case 2:
            shooting_tracking(enemy, ship, dt, gFrame); 
            break;
        case 3:
            move_enemy_mirror(enemy, ship);
            shooting_horizontal(enemy, ship, dt, gFrame); 
            break;
        case 7:
            shooting_tracking(enemy, ship, dt, gFrame); 
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
            shooting_horizontal(enemy, ship, dt, gFrame);
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
            move_enemy_carried_element(enemy, ship);
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

#endif
