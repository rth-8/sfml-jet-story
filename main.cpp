#include <cstdlib>
#include <ctime>
#include <iostream>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "assets.h"
#include "sounds.h"
#include "maze_data.h"
#include "infobar.h"
#include "explosions.h"
#include "projectiles.h"
#include "ship.h"
#include "maze.h"
#include "menu.h"
#include "scene_game.h"

enum Scenes
{
    SCENE_TITLE,
    SCENE_MAIN_MENU,
    SCENE_GAME,
};

void new_game(Ship & ship, Maze & maze, const MazeData & mazeData, const Assets & assets)
{
    maze.rooms.clear();
    maze.current_room_row = 0;
    maze.current_room_col = 0;
    maze.base_cnt = 0;
    create_maze(maze, mazeData, assets);
    reset_ship(ship, {200,310});
}

int main()
{
    std::srand(std::time({}));

    sf::RenderWindow window(sf::VideoMode({800, 600}), "js");
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);

    sf::Clock clock;
    int game_frame = 0;

    Scenes current_scene = SCENE_TITLE;

    Assets assets;
    load_wall_textures(assets);
    load_item_textures(assets);
    load_ship_textures(assets);
    load_enemy_textures(assets);
    load_enemy_shot_textures(assets);
    load_misc_textures(assets);
    load_sounds(assets);
    load_font(assets);
    
    MazeData mazeData;
    load_maze(mazeData);
    
    Maze maze;
    
    Ship ship;
    create_ship(ship, assets);

    Projectiles projectiles;
    
    Explosions explosions;
    explosions.flashing_counter = 0;
    
    Sounds sounds;
    create_sounds(sounds, assets);

    InfoBar infobar;
    create_infobar(infobar, assets);

    sf::Texture titleTex("./images/misc/loadscr.png");
    sf::Sprite titleSpr(titleTex);

    Menu mainMenu;
    add_button(mainMenu, "New Game", {10,10}, {500,50}, assets.font);
    add_button(mainMenu, "Continue", {10,mainMenu.buttons[mainMenu.buttons.size()-1].rect.getPosition().y + 50 + 10}, {500,50}, assets.font);
    add_button(mainMenu, "Load",     {10,mainMenu.buttons[mainMenu.buttons.size()-1].rect.getPosition().y + 50 + 10}, {500,50}, assets.font);
    add_button(mainMenu, "Save",     {10,mainMenu.buttons[mainMenu.buttons.size()-1].rect.getPosition().y + 50 + 10}, {500,50}, assets.font);
    add_button(mainMenu, "Quit",     {10,mainMenu.buttons[mainMenu.buttons.size()-1].rect.getPosition().y + 50 + 10}, {500,50}, assets.font);
    
    mainMenu.current = 0;
    mainMenu.buttons[mainMenu.current].rect.setOutlineColor(sf::Color::Yellow);

    mainMenu.buttons[0].callback = [&ship, &maze, &mazeData, &assets, &current_scene] () {
        new_game(ship, maze, mazeData, assets);
        current_scene = SCENE_GAME;
    };

    mainMenu.buttons[4].callback = [&window] () {
        window.close();
    };

    while (window.isOpen())
    {
        sf::Time dt = clock.restart();
        float dtAsSeconds = dt.asSeconds();
        // std::cout << "dt = " << dtAsSeconds << "\n";
        
        // SFML events
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            if (current_scene == SCENE_MAIN_MENU)
            {
                menu_input(mainMenu, event);
            }
        }

        if (current_scene == SCENE_TITLE)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter))
            {
                current_scene = SCENE_MAIN_MENU;
            }
            window.clear();
            window.draw(titleSpr);
            window.display();
        }
        else
        if (current_scene == SCENE_MAIN_MENU)
        {
            window.clear();
            draw_menu(window, mainMenu);
            window.display();
        }
        else
        if (current_scene == SCENE_GAME)
        {
            scene_game_input(ship, sounds, assets, dtAsSeconds);
            scene_game_update(ship, maze, projectiles, explosions, sounds, mazeData, assets, dtAsSeconds, game_frame);
            if (scene_game_collisions(ship, maze, projectiles, explosions, sounds, assets))
            {
                game_frame = 0;
                continue;
            }
            scene_game_animations(ship, maze, projectiles, explosions, dtAsSeconds);
            update_infobar(infobar, maze, ship, assets, game_frame);
            scene_game_draw(window, ship, maze, projectiles, explosions, infobar, game_frame);
            scene_game_cleanup(maze, projectiles, explosions);
        }

        game_frame++;
    }

    return 0;
}
