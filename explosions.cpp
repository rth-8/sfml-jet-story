#include "explosions.h"
#include "common.h"

void create_fragment(Explosions & explosions, const sf::Vector2f & pos, const sf::Vector2f & velocity, const Assets & assets)
{
    Fragment fg;
    create_animation(fg.anim, (uint16_t)0, assets.fragment, 24, 24, 5, 20);
    fg.anim.sprite.value().setPosition(pos);
    set_frame(fg.anim, std::rand() % fg.anim.frame_count);
    fg.velocity = velocity;
    explosions.fragments.push_back(fg);
}

void create_explosion(Explosions & explosions, const sf::Vector2f & pos, const Assets & assets)
{
    Animation expl;
    create_animation(expl, (uint16_t)0, assets.explosion, 50, 50, 6, 6);
    expl.sprite.value().setPosition(pos);
    explosions.explosions.push_back(expl);

    /* 1 2 3
       4   5
       6 7 8 */
    create_fragment(explosions, pos, {(std::rand() % (200-150+1) + 150) * -1.0f, (std::rand() % (200-150+1) + 150) * -1.0f}, assets);
    create_fragment(explosions, pos, {0.0f, (std::rand() % (200-150+1) + 150) * -1.0f}, assets);
    create_fragment(explosions, pos, {(std::rand() % (200-150+1) + 150) * 1.0f, (std::rand() % (200-150+1) + 150) * -1.0f}, assets);
    create_fragment(explosions, pos, {(std::rand() % (200-150+1) + 150) * -1.0f, 0.0f}, assets);
    create_fragment(explosions, pos, {(std::rand() % (200-150+1) + 150) * 1.0f, 0.0f}, assets);
    create_fragment(explosions, pos, {(std::rand() % (200-150+1) + 150) * -1.0f, (std::rand() % (200-150+1) + 150) * 1.0f}, assets);
    create_fragment(explosions, pos, {0.0f, (std::rand() % (200-150+1) + 150) * 1.0f}, assets);
    create_fragment(explosions, pos, {(std::rand() % (200-150+1) + 150) * 1.0f, (std::rand() % (200-150+1) + 150) * 1.0f}, assets);
}

void move_fragment(Fragment & fg, float dt)
{
    fg.velocity.y += GRAVITY;
    fg.anim.sprite.value().move(fg.velocity * dt);
}

void fragment_check_bounds(Fragment & fg)
{
    if (fg.anim.sprite.value().getPosition().x < EDGE_LEFT + fg.anim.half_size.x ||
        fg.anim.sprite.value().getPosition().x > EDGE_RIGHT - fg.anim.half_size.x ||
        fg.anim.sprite.value().getPosition().y < EDGE_TOP + fg.anim.half_size.y ||
        fg.anim.sprite.value().getPosition().y > EDGE_BOTTOM - fg.anim.half_size.y)
    {
        fg.anim.isAlive = false;
    }
}
