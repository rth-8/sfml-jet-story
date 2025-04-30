#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>

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

#define WINDOW_W 800
#define WINDOW_H 600

enum Scenes
{
    SCENE_TITLE,
    SCENE_MAIN_MENU,
    SCENE_GAME,
    SCENE_LOAD,
    SCENE_SAVE
};

#define MAIN_MENU_X 150
#define MAIN_MENU_Y 230
#define MAIN_MENU_BTN_W 500
#define MAIN_MENU_BTN_H 50
#define MAIN_MENU_V_PAD 20
#define MAIN_MENU_CHAR_SIZE 32

void new_game(Ship & ship, Maze & maze, const MazeData & mazeData, const Assets & assets)
{
    maze.rooms.clear();
    maze.current_room_row = 0;
    maze.current_room_col = 0;
    maze.base_cnt = 0;
    create_maze(maze, mazeData, assets);
    get_current_room(maze).visited = true;
    maze.created = true;
    reset_ship(ship, {200,310});
}

void setup_main_menu(Menu & m, sf::RenderWindow & window, Ship & ship, Maze & maze, 
    const MazeData & mazeData, const Assets & assets, Scenes & current_scene)
{
    const auto btnSize = sf::Vector2f(MAIN_MENU_BTN_W, MAIN_MENU_BTN_H);
    add_button(m, "New Game", {MAIN_MENU_X, MAIN_MENU_Y}, btnSize, assets.font_menu, MAIN_MENU_CHAR_SIZE);
    add_button(m, "Continue", {MAIN_MENU_X, m.buttons[m.buttons.size()-1].rect.getPosition().y + MAIN_MENU_BTN_H + MAIN_MENU_V_PAD}, btnSize, assets.font_menu, MAIN_MENU_CHAR_SIZE);
    add_button(m, "Load",     {MAIN_MENU_X, m.buttons[m.buttons.size()-1].rect.getPosition().y + MAIN_MENU_BTN_H + MAIN_MENU_V_PAD}, btnSize, assets.font_menu, MAIN_MENU_CHAR_SIZE);
    add_button(m, "Save",     {MAIN_MENU_X, m.buttons[m.buttons.size()-1].rect.getPosition().y + MAIN_MENU_BTN_H + MAIN_MENU_V_PAD}, btnSize, assets.font_menu, MAIN_MENU_CHAR_SIZE);
    add_button(m, "Quit",     {MAIN_MENU_X, m.buttons[m.buttons.size()-1].rect.getPosition().y + MAIN_MENU_BTN_H + MAIN_MENU_V_PAD}, btnSize, assets.font_menu, MAIN_MENU_CHAR_SIZE);
    m.current = 0;
    m.buttons[m.current].rect.setOutlineColor(sf::Color::Yellow);

    m.buttons[0].callback = [&ship, &maze, &mazeData, &assets, &current_scene] () {
        new_game(ship, maze, mazeData, assets);
        current_scene = SCENE_GAME;
    };

    m.buttons[1].callback = [&maze, &current_scene] () {
        if (maze.created) current_scene = SCENE_GAME;
    };

    m.buttons[2].callback = [&maze, &current_scene] () {
        current_scene = SCENE_LOAD;
    };

    m.buttons[3].callback = [&maze, &current_scene] () {
        if (maze.created) current_scene = SCENE_SAVE;
    };

    m.buttons[4].callback = [&window] () {
        window.close();
    };
}

void setup_slots_menu(Menu & m, Maze & maze, Ship & ship, const MazeData & mazeData, const Assets & assets, Scenes & current_scene)
{
    const auto btnSize = sf::Vector2f(MAIN_MENU_BTN_W, MAIN_MENU_BTN_H);

    for (int i=0; i<5; ++i)
    {
        std::ifstream input;
        input.open(std::format("./save{:03d}m", i), std::ios::binary);
        std::string btnName = "---";
        if (input.is_open())
        {
            uint16_t baseCnt;
            input.read(reinterpret_cast <char*>(&baseCnt), sizeof(uint16_t));
            // std::cout << "max: " << mazeData.base_cnt << "\n";
            // std::cout << "slot: " << baseCnt << "\n";
            int prc = ((float)(mazeData.base_cnt - baseCnt) / (float)mazeData.base_cnt) * 100.0f;
            btnName = std::format("{:d} ... {: d}%", (i+1), prc);
            input.close();
        }

        if (i == 0)
        {
            add_button(m, std::move(btnName), {MAIN_MENU_X, MAIN_MENU_Y}, btnSize, assets.font_menu, MAIN_MENU_CHAR_SIZE);
        }
        else
        {
            add_button(m, std::move(btnName), 
                {MAIN_MENU_X, m.buttons[m.buttons.size()-1].rect.getPosition().y + MAIN_MENU_BTN_H + MAIN_MENU_V_PAD}, 
                btnSize, assets.font_menu, MAIN_MENU_CHAR_SIZE);
        }
    }

    m.current = 0;
    m.buttons[m.current].rect.setOutlineColor(sf::Color::Yellow);

    for (auto & btn : m.buttons)
    {
        btn.callback = [&maze, &ship, &mazeData, &assets, &m, &current_scene] () {
            if (current_scene == SCENE_LOAD)
            {
                load_maze(maze, mazeData, assets, m.current);
                load_ship(ship, m.current);
            }
            else
            if (current_scene == SCENE_SAVE)
            {
                save_maze(maze, m.current);
                save_ship(ship, m.current);
                int prc = ((float)(mazeData.base_cnt - maze.base_cnt) / (float)mazeData.base_cnt) * 100.0f;
                m.buttons[m.current].text.value().setString(std::format("{:d} ... {: d}%", (m.current+1), prc));
            }
        };
    }
}

int main()
{
    std::srand(std::time({}));

    sf::RenderWindow window(sf::VideoMode({WINDOW_W, WINDOW_H}), "js");
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
    load_maze_data(mazeData);
    
    Maze maze;
    maze.created = false;
    
    Ship ship;
    create_ship(ship, assets);

    Projectiles projectiles;
    
    Explosions explosions;
    explosions.flashing_counter = 0;
    
    Sounds sounds;
    create_sounds(sounds, assets);

    InfoBar infobar;
    create_infobar(infobar, assets);

    sf::Sprite loadingScreenSpr(assets.loadscr);
    sf::Sprite titleSpr(assets.title);

    Menu mainMenu;
    setup_main_menu(mainMenu, window, ship, maze, mazeData, assets, current_scene);

    Menu slotsMenu;
    setup_slots_menu(slotsMenu, maze, ship, mazeData, assets, current_scene);

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
            else
            if (current_scene == SCENE_GAME)
            {
                if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
                {
                    if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    {
                        mainMenu.buttons[mainMenu.current].rect.setOutlineColor(sf::Color::White);
                        mainMenu.current = 1;
                        mainMenu.buttons[mainMenu.current].rect.setOutlineColor(sf::Color::Yellow);
                        current_scene = SCENE_MAIN_MENU;
                    }
                }
            }
            else
            if (current_scene == SCENE_LOAD || current_scene == SCENE_SAVE)
            {
                menu_input(slotsMenu, event);

                if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
                {
                    if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    {
                        current_scene = SCENE_MAIN_MENU;
                    }
                }
            }
        }

        if (current_scene == SCENE_TITLE)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter))
            {
                current_scene = SCENE_MAIN_MENU;
            }
            window.clear();
            window.draw(loadingScreenSpr);
            window.display();
        }
        else
        if (current_scene == SCENE_MAIN_MENU)
        {
            window.clear();
            window.draw(titleSpr);
            draw_menu(window, mainMenu);
            window.display();
        }
        else
        if (current_scene == SCENE_LOAD || current_scene == SCENE_SAVE)
        {
            window.clear();
            draw_menu(window, slotsMenu);
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
