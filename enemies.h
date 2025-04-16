#ifndef ENEMIES_H
#define ENEMIES_H

#include <set>

namespace js {
namespace GameObjects {

const std::set<int> non_animated{0, 1, 2, 3, 4, 5, 6, 8, 11, 18};

struct Enemy
{
    Animation anim;
    std::optional<Animation> carried_enemy;
    std::optional<Animation> carried_item;
};

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

void create_enemy(Enemy & eo, const js::Data::Enemy & enemy, const Assets & assets)
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

} // namespace GameObjects
} // namespace js

#endif
