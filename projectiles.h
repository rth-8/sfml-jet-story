#ifndef PROJECTILES_H
#define PROJECTILES_H

typedef struct
{
    Animation anim;
    sf::Vector2f velocity;
}
Projectile;

std::vector<Projectile> projectiles;

Projectile & create_projectile(const Assets & assets, int id)
{
    Projectile prj;
    auto & tex = assets.enemy_shots.at(id);
    switch (id)
    {
        case 1:  create_animation(prj.anim, id, tex, 50, 50, 4, 6); break;
        case 2:  create_animation(prj.anim, id, tex, tex.getSize().x, tex.getSize().y); break;
        case 3:  create_animation(prj.anim, id, tex, tex.getSize().x, tex.getSize().y); break;
        case 5:  create_animation(prj.anim, id, tex, 40, 50, 4, 6); break;
        case 6:  create_animation(prj.anim, id, tex, 34, 50, 4, 6); break;
        case 7:  create_animation(prj.anim, id, tex, 18, 21, 4, 6); break;
        case 8:  create_animation(prj.anim, id, tex, tex.getSize().x, tex.getSize().y); break;
        case 9:  create_animation(prj.anim, id, tex, 50, 35, 2, 6); break;
        case 13: create_animation(prj.anim, id, tex, tex.getSize().x, tex.getSize().y); break;
    }
    projectiles.push_back(prj);
    return projectiles[projectiles.size()-1];
}

void collision_projectile_wall(Projectile & prj, Animation & wall, const Assets & assets)
{
    if (checkCollision(prj.anim, wall))
    {
        prj.anim.isAlive = false;

        if (prj.anim.id == 1 || prj.anim.id == 5 || prj.anim.id == 6 || prj.anim.id == 8 || prj.anim.id == 9)
        {
            sound_boom.value().play();
            create_explosion(prj.anim.sprite.value().getPosition(), assets);
        }
    }
}

void collision_projectile_ship(Projectile & prj, Ship & ship, const Assets & assets)
{
    if (checkCollision(prj.anim, ship.ship_body))
    {
        prj.anim.isAlive = false;

        if (prj.anim.id == 9)
        {
            ship.shield = 0;
        }
        else
        {
            ship.shield -= 10;
        }

        if (prj.anim.id == 1 || prj.anim.id == 5 || prj.anim.id == 6 || prj.anim.id == 8 || prj.anim.id == 9)
        {
            sound_boom.value().play();
            create_explosion(prj.anim.sprite.value().getPosition(), assets);
        }
    }
}

void projectile_check_bounds(Projectile & prj)
{
    if (prj.anim.sprite.value().getPosition().x < EDGE_LEFT + prj.anim.half_size.x ||
        prj.anim.sprite.value().getPosition().x > EDGE_RIGHT - prj.anim.half_size.x ||
        prj.anim.sprite.value().getPosition().y < EDGE_TOP + prj.anim.half_size.y ||
        prj.anim.sprite.value().getPosition().y > EDGE_BOTTOM - prj.anim.half_size.y)
    {
        prj.anim.isAlive = false;
    }   
}

#endif
