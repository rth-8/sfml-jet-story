#ifndef SCENE_GAME_H
#define SCENE_GAME_H

#include <SFML/Graphics/RenderWindow.hpp>

struct MazeData;
struct Maze;
struct Ship;
struct Projectiles;
struct Explosions;
struct Sounds;
struct Assets;
struct InfoBar;

void scene_game_input(Ship & ship, Sounds & sounds, const Assets & assets, float dt);

void scene_game_update(Ship & ship, Maze & maze, Projectiles & projectiles, Explosions & explosions, Sounds & sounds, 
    const MazeData & mazeData, const Assets & assets, float dt, int gFrame);

void scene_game_clear_all(Projectiles & projectiles, Explosions & explosions, Sounds & sounds);

bool scene_game_collisions(Ship & ship, Maze & maze, Projectiles & projectiles, Explosions & explosions, Sounds & sounds, 
    const Assets & assets);

void scene_game_animations(Ship & ship, Maze & maze, Projectiles & projectiles, Explosions & explosions, float dt);

void scene_game_draw(sf::RenderWindow & window, Ship & ship, Maze & maze, Projectiles & projectiles, Explosions & explosions, 
    InfoBar & infobar, int gFrame);

void scene_game_cleanup(Maze & maze, Projectiles & projectiles, Explosions & explosions);

#endif
