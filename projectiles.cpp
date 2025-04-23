#include "projectiles.h"
#include "common.h"
#include "assets.h"
#include "physics.h"
#include "explosions.h"
#include "ship.h"
#include "sounds.h"

Projectile & create_projectile(Projectiles & projectiles, Sounds & sounds, const Assets & assets, int id)
{
    Projectile prj;
    auto & tex = assets.enemy_shots.at(id);
    switch (id)
    {
        case 1:  
            create_animation(prj.anim, id, tex, 50, 50, 4, 6); 
            sounds.sounds.at(ENEMY_01_SHOT).play();
            break;
        case 2:  
            create_animation(prj.anim, id, tex, tex.getSize().x, tex.getSize().y); 
            sounds.sounds.at(ENEMY_02_SHOT).play();
            break;
        case 3:  
            create_animation(prj.anim, id, tex, tex.getSize().x, tex.getSize().y); 
            sounds.sounds.at(ENEMY_03_13_SHOT).play();
            break;
        case 5:  
            create_animation(prj.anim, id, tex, 40, 50, 4, 6); 
            sounds.sounds.at(ENEMY_05_SHOT).play();
            break;
        case 6:  
            create_animation(prj.anim, id, tex, 34, 50, 4, 6); 
            sounds.sounds.at(ENEMY_06_SHOT).play();
            break;
        case 7:  
            create_animation(prj.anim, id, tex, 18, 21, 4, 6); 
            sounds.sounds.at(ENEMY_07_SHOT).play();
            break;
        case 8:  
            create_animation(prj.anim, id, tex, tex.getSize().x, tex.getSize().y); 
            sounds.sounds.at(ENEMY_08_SHOT).play();
            break;
        case 9:  
            create_animation(prj.anim, id, tex, 50, 35, 2, 6); 
            sounds.sounds.at(ENEMY_09_LAUNCH).play();
            break;
        case 13: 
            create_animation(prj.anim, id, tex, tex.getSize().x, tex.getSize().y); 
            sounds.sounds.at(ENEMY_03_13_SHOT).play();
            break;
    }
    projectiles.projectiles.push_back(prj);
    return projectiles.projectiles[projectiles.projectiles.size()-1];
}

void move_projectile(Projectile & prj, Sounds & sounds, float dt)
{
    prj.anim.sprite.value().move(prj.velocity * dt);

    if (prj.anim.id == 1 && sounds.sounds.at(ENEMY_01_SHOT).getStatus() != sf::SoundSource::Status::Playing)
    {
        sounds.sounds.at(ENEMY_01_SHOT).play();
    }

    if (prj.anim.id == 5 && sounds.sounds.at(ENEMY_05_SHOT).getStatus() != sf::SoundSource::Status::Playing)
    {
        sounds.sounds.at(ENEMY_05_SHOT).play();
    }

    if (prj.anim.id == 6 && sounds.sounds.at(ENEMY_06_SHOT).getStatus() != sf::SoundSource::Status::Playing)
    {
        sounds.sounds.at(ENEMY_06_SHOT).play();
    }

    if (prj.anim.id == 8 && sounds.sounds.at(ENEMY_08_SHOT).getStatus() != sf::SoundSource::Status::Playing)
    {
        sounds.sounds.at(ENEMY_08_SHOT).play();
    }

    if (prj.anim.id == 9 && sounds.sounds.at(ENEMY_09_SHOT).getStatus() != sf::SoundSource::Status::Playing)
    {
        sounds.sounds.at(ENEMY_09_SHOT).play();
    }
}

void collision_projectile_wall(Projectile & prj, Animation & wall, Explosions & explosions, Sounds & sounds, const Assets & assets)
{
    if (checkCollision(prj.anim, wall))
    {
        prj.anim.isAlive = false;

        if (prj.anim.id == 1 || prj.anim.id == 5 || prj.anim.id == 6 || prj.anim.id == 8 || prj.anim.id == 9)
        {
            sounds.sounds.at(BOOM).play();
            create_explosion(explosions, prj.anim.sprite.value().getPosition(), assets);
        }

        if (prj.anim.id == 1)
        {
            sounds.sounds.at(ENEMY_01_SHOT).stop();
        }

        if (prj.anim.id == 5)
        {
            sounds.sounds.at(ENEMY_05_SHOT).stop();
        }

        if (prj.anim.id == 6)
        {
            sounds.sounds.at(ENEMY_06_SHOT).stop();
        }

        if (prj.anim.id == 8)
        {
            sounds.sounds.at(ENEMY_08_SHOT).stop();
        }

        if (prj.anim.id == 9)
        {
            sounds.sounds.at(ENEMY_09_SHOT).stop();
        }
    }
}

void collision_projectile_ship(Projectile & prj, Ship & ship, Explosions & explosions, Sounds & sounds, const Assets & assets)
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

        sounds.sounds.at(HIT_SHIP).play();

        if (prj.anim.id == 1 || prj.anim.id == 5 || prj.anim.id == 6 || prj.anim.id == 8 || prj.anim.id == 9)
        {
            sounds.sounds.at(BOOM).play();
            create_explosion(explosions, prj.anim.sprite.value().getPosition(), assets);
        }

        if (prj.anim.id == 1)
        {
            sounds.sounds.at(ENEMY_01_SHOT).stop();
        }

        if (prj.anim.id == 5)
        {
            sounds.sounds.at(ENEMY_05_SHOT).stop();
        }

        if (prj.anim.id == 6)
        {
            sounds.sounds.at(ENEMY_06_SHOT).stop();
        }

        if (prj.anim.id == 8)
        {
            sounds.sounds.at(ENEMY_08_SHOT).stop();
        }

        if (prj.anim.id == 9)
        {
            sounds.sounds.at(ENEMY_09_SHOT).stop();
        }
    }
}

void projectile_check_bounds(Projectile & prj, Sounds & sounds)
{
    if (prj.anim.sprite.value().getPosition().x < EDGE_LEFT + prj.anim.half_size.x ||
        prj.anim.sprite.value().getPosition().x > EDGE_RIGHT - prj.anim.half_size.x ||
        prj.anim.sprite.value().getPosition().y < EDGE_TOP + prj.anim.half_size.y ||
        prj.anim.sprite.value().getPosition().y > EDGE_BOTTOM - prj.anim.half_size.y)
    {
        prj.anim.isAlive = false;

        if (prj.anim.id == 1)
        {
            sounds.sounds.at(ENEMY_01_SHOT).stop();
        }

        if (prj.anim.id == 5)
        {
            sounds.sounds.at(ENEMY_05_SHOT).stop();
        }

        if (prj.anim.id == 6)
        {
            sounds.sounds.at(ENEMY_06_SHOT).stop();
        }
        
        if (prj.anim.id == 8)
        {
            sounds.sounds.at(ENEMY_08_SHOT).stop();
        }

        if (prj.anim.id == 9)
        {
            sounds.sounds.at(ENEMY_09_SHOT).stop();
        }
    }   
}
