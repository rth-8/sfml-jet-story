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

#endif
