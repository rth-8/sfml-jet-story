#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <cmath>

#include <SFML/Window.hpp>
#include <SFML/Window/VideoMode.hpp>
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

#define WINDOW_CX 400
#define WINDOW_CY 300

enum Scenes
{
    SCENE_TITLE,
    SCENE_MAIN_MENU,
    SCENE_GAME,
    SCENE_LOAD,
    SCENE_SAVE,
    SCENE_GAME_OVER,
    SCENE_NO_BASES,
    SCENE_WIN,
};

enum MainMenuButtons
{
    BTN_NEW_GAME = 0,
    BTN_CONTINUE,
    BTN_LOAD,
    BTN_SAVE,
    BTN_SETTINGS,
    BTN_QUIT,
};

#define MAIN_MENU_X 150
#define MAIN_MENU_Y 230
#define MAIN_MENU_BTN_W 500
#define MAIN_MENU_BTN_H 40
#define MAIN_MENU_V_PAD 22
#define MAIN_MENU_CHAR_SIZE 24

static int transition = 0;

void new_game(Ship & ship, Maze & maze, const MazeData & mazeData, const Assets & assets)
{
    maze.rooms.clear();
    maze.current_room_row = 0;
    maze.current_room_col = 0;
    maze.base_cnt = 0;
    maze.score = 0;
    create_maze(maze, mazeData, assets);
    get_current_room(maze).visited = true;
    maze.created = true;
    reset_ship(ship, {200,310});
}

bool load_game(Maze & maze, Ship & ship, const MazeData & mazeData, const Assets & assets, int slot)
{
    bool r1 = load_maze(maze, mazeData, assets, slot);
    bool r2 = load_ship(ship, slot);
    return r1 && r2;
}

bool save_game(Maze & maze, Ship & ship, int slot)
{
    bool r1 = save_maze(maze, slot);
    bool r2 = save_ship(ship, slot);
    return r1 && r2;
}

void setup_main_menu(Menu & m, sf::RenderWindow & window, Ship & ship, Maze & maze, 
    const MazeData & mazeData, const Assets & assets, Scenes & current_scene)
{
    const auto btnSize = sf::Vector2f(MAIN_MENU_BTN_W, MAIN_MENU_BTN_H);
    add_button(m, "New Game", {MAIN_MENU_X, MAIN_MENU_Y}, btnSize, assets.font_menu, MAIN_MENU_CHAR_SIZE);
    add_button(m, "Continue", {MAIN_MENU_X, m.buttons[m.buttons.size()-1].rect.getPosition().y + MAIN_MENU_BTN_H + MAIN_MENU_V_PAD}, btnSize, assets.font_menu, MAIN_MENU_CHAR_SIZE);
    add_button(m, "Load",     {MAIN_MENU_X, m.buttons[m.buttons.size()-1].rect.getPosition().y + MAIN_MENU_BTN_H + MAIN_MENU_V_PAD}, btnSize, assets.font_menu, MAIN_MENU_CHAR_SIZE);
    add_button(m, "Save",     {MAIN_MENU_X, m.buttons[m.buttons.size()-1].rect.getPosition().y + MAIN_MENU_BTN_H + MAIN_MENU_V_PAD}, btnSize, assets.font_menu, MAIN_MENU_CHAR_SIZE);
    add_button(m, "Settings", {MAIN_MENU_X, m.buttons[m.buttons.size()-1].rect.getPosition().y + MAIN_MENU_BTN_H + MAIN_MENU_V_PAD}, btnSize, assets.font_menu, MAIN_MENU_CHAR_SIZE);
    add_button(m, "Quit",     {MAIN_MENU_X, m.buttons[m.buttons.size()-1].rect.getPosition().y + MAIN_MENU_BTN_H + MAIN_MENU_V_PAD}, btnSize, assets.font_menu, MAIN_MENU_CHAR_SIZE);
    
    m.current = BTN_NEW_GAME;
    m.buttons[m.current].selected = true;
    m.buttons[BTN_CONTINUE].enabled = false;
    m.buttons[BTN_SAVE].enabled = false;

    m.buttons[BTN_NEW_GAME].callback = [&ship, &maze, &mazeData, &assets, &m, &current_scene] () {
        new_game(ship, maze, mazeData, assets);
        m.buttons[BTN_CONTINUE].enabled = true;
        m.buttons[BTN_SAVE].enabled = true;
        current_scene = SCENE_GAME;
        transition = 10;
    };

    m.buttons[BTN_CONTINUE].callback = [&maze, &current_scene] () {
        if (maze.created)
        {
            current_scene = SCENE_GAME;
            transition = 10;
        }
    };

    m.buttons[BTN_LOAD].callback = [&maze, &current_scene] () {
        current_scene = SCENE_LOAD;
    };

    m.buttons[BTN_SAVE].callback = [&maze, &current_scene] () {
        if (maze.created) current_scene = SCENE_SAVE;
    };

    m.buttons[BTN_QUIT].callback = [&window] () {
        window.close();
    };
}

void back_to_main_menu_continue(Menu & m)
{
    m.buttons[m.current].selected = false;
    m.current = BTN_CONTINUE;
    m.buttons[m.current].selected = true;
}

void setup_slots_menu(Menu & m, Menu & mainMenu, Maze & maze, Ship & ship, const MazeData & mazeData, 
    const Assets & assets, Scenes & current_scene)
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
            btnName = std::format("{:d} ... {: 3d}%", (i+1), prc);
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
    m.buttons[m.current].selected = true;

    for (auto & btn : m.buttons)
    {
        btn.callback = [&maze, &ship, &mazeData, &assets, &m, &mainMenu, &current_scene] () {
            if (current_scene == SCENE_LOAD)
            {
                if (load_game(maze, ship, mazeData, assets, m.current))
                {
                    back_to_main_menu_continue(mainMenu);
                    mainMenu.buttons[BTN_CONTINUE].enabled = true;
                    mainMenu.buttons[BTN_SAVE].enabled = true;
                    current_scene = SCENE_MAIN_MENU;
                }
            }
            else
            if (current_scene == SCENE_SAVE)
            {
                if (save_game(maze, ship, m.current))
                {
                    int prc = ((float)(mazeData.base_cnt - maze.base_cnt) / (float)mazeData.base_cnt) * 100.0f;
                    set_button_string(m.buttons[m.current], std::format("{:d} ... {: 3d}%", (m.current+1), prc));
                    back_to_main_menu_continue(mainMenu);
                    current_scene = SCENE_MAIN_MENU;
                }
            }
        };
    }
}

void update_view(sf::RenderWindow & window)
{
    printf("Window: %d x %d\n", window.getSize().x, window.getSize().y);
    printf("View  : %f x %f\n", window.getView().getSize().x, window.getView().getSize().y);

    // sf::View view({WINDOW_CX, WINDOW_CY}, {WINDOW_W, WINDOW_H});
    // sf::View view({WINDOW_CX, WINDOW_CY}, sf::Vector2f(window.getSize()));

    float w = 0;
    float h = 0;
    if (window.getSize().x > window.getSize().y)
    {
        printf("LANDSCAPE\n");
        float ratio = (float)window.getSize().x / (float)window.getSize().y;
        w = ratio * WINDOW_H;
        h = WINDOW_H;
    }
    else
    {
        printf("PORTRAIT\n");
        float ratio = (float)window.getSize().x / (float)window.getSize().y;
        w = WINDOW_W;
        h = WINDOW_W / ratio;
    }
    printf("New view: %f x %f\n", w, h);
    sf::View view({WINDOW_CX, WINDOW_CY}, {w, h});

    window.setView(view);
}

int main()
{
    std::srand(std::time({}));

    auto & modes = sf::VideoMode::getFullscreenModes();
    for (auto & mode : modes)
    {
        printf("Mode: %d x %d (%s)\n", mode.size.x, mode.size.y, mode.isValid() ? "OK" : "x");
    }

    sf::RenderWindow window(sf::VideoMode({WINDOW_W, WINDOW_H}), "js", sf::State::Windowed);
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);
    update_view(window);
    bool isFullscreen = false;

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
    setup_slots_menu(slotsMenu, mainMenu, maze, ship, mazeData, assets, current_scene);

    sf::Text loadText(assets.font_menu);
    loadText.setString("Load game");
    loadText.setPosition({400.0f - loadText.getGlobalBounds().size.x * 0.5f, 150.f});

    sf::Text saveText(assets.font_menu);
    saveText.setString("Save game");
    saveText.setPosition({400.0f - saveText.getGlobalBounds().size.x * 0.5f, 150.f});

    sf::Text gameOverText(assets.font_menu);
    sf::Text finalScoreText(assets.font_menu);
    sf::Text finalPrcText(assets.font_menu);
    gameOverText.setString("Game over!");
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setPosition({400.0f - gameOverText.getGlobalBounds().size.x * 0.5f, 150.f});

    sf::Text winText(assets.font_menu);
    winText.setString("Mission accomplished!");
    winText.setFillColor(sf::Color::Green);
    winText.setPosition({400.0f - winText.getGlobalBounds().size.x * 0.5f, 150.f});
    int end_game_booms = 0;

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
            else
            if (event->is<sf::Event::Resized>())
            {
                update_view(window);
            }
            else
            if (const auto* kr = event->getIf<sf::Event::KeyReleased>())
            {
                if (kr->scancode == sf::Keyboard::Scancode::Enter && kr->alt)
                {
                    if (isFullscreen)
                    {
                        // go to windowed
                        window.create(sf::VideoMode({WINDOW_W, WINDOW_H}), "js", sf::State::Windowed);
                        window.setFramerateLimit(60);
                        window.setKeyRepeatEnabled(false);
                        update_view(window);
                        isFullscreen = false;
                    }
                    else
                    {
                        // go to fullscreen
                        // TODO: fullscreen resolution will be part of setting
                        window.create(sf::VideoMode({1920, 1080}), "js", sf::State::Fullscreen);
                        window.setFramerateLimit(60);
                        window.setKeyRepeatEnabled(false);
                        update_view(window);
                        isFullscreen = true;
                    }
                }
            }

            if (current_scene == SCENE_TITLE || current_scene == SCENE_GAME_OVER || current_scene == SCENE_WIN)
            {
                if (sounds.sounds.at(DEATH).getStatus() != sf::SoundSource::Status::Playing)
                {
                    // Any key or joystick button to return to main menu
                    if (event->getIf<sf::Event::KeyPressed>())
                    {
                        current_scene = SCENE_MAIN_MENU;
                    }
                    if (event->getIf<sf::Event::JoystickButtonPressed>())
                    {
                        current_scene = SCENE_MAIN_MENU;
                    }
                }
            }
            else
            if (current_scene == SCENE_MAIN_MENU)
            {
                if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
                {
                    if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    {
                        window.close();
                    }
                }
                menu_input(mainMenu, event);
            }
            else
            if (current_scene == SCENE_GAME)
            {
                // ESC to main menu
                if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
                {
                    if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    {
                        back_to_main_menu_continue(mainMenu);
                        current_scene = SCENE_MAIN_MENU;
                    }
                }
                // Button 8 or 9 to main menu (Start or Select on Xbox controller)
                if (const auto* joystickButtonPressed = event->getIf<sf::Event::JoystickButtonPressed>())
                {
                    if (joystickButtonPressed->button == 8 || joystickButtonPressed->button == 9)
                    {
                        back_to_main_menu_continue(mainMenu);
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

                if (const auto* joystickButtonPressed = event->getIf<sf::Event::JoystickButtonPressed>())
                {
                    if (joystickButtonPressed->button == 1)
                    {
                        current_scene = SCENE_MAIN_MENU;
                    }
                }
            }

            // test
            // if (const auto* joystickButtonPressed = event->getIf<sf::Event::JoystickButtonPressed>())
            // {
            //     int btn = static_cast<int>(joystickButtonPressed->button);
            //     printf("Joystick button: %d\n", btn);
            // }
            // if (const auto* joystickMoved = event->getIf<sf::Event::JoystickMoved>())
            // {
            //     printf("Joystick: axis %d, position %f\n", (int)joystickMoved->axis, joystickMoved->position);
            // }
        }

        if (current_scene == SCENE_TITLE)
        {
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
            if (current_scene == SCENE_LOAD) window.draw(loadText);
            else if (current_scene == SCENE_SAVE) window.draw(saveText);
            draw_menu(window, slotsMenu);
            window.display();
        }
        else
        if (current_scene == SCENE_GAME_OVER)
        {
            if (sounds.sounds.at(DEATH).getStatus() == sf::SoundSource::Status::Playing)
            {
                scene_game_draw(window, ship, maze, projectiles, explosions, infobar, game_frame);
            }
            else
            {
                window.clear();
                window.draw(gameOverText);
                window.draw(finalScoreText);
                window.draw(finalPrcText);
                window.display();
            }
        }
        else
        if (current_scene == SCENE_WIN)
        {
            window.clear();
            window.draw(winText);
            window.draw(finalScoreText);
            window.display();
        }
        else
        if (current_scene == SCENE_GAME || current_scene == SCENE_NO_BASES)
        {
            if (!transition && current_scene == SCENE_GAME)
            {
                scene_game_input(ship, sounds, assets, dtAsSeconds);
            }
            else
            {
                transition--;
            }
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

            if (ship.shield <= 0)
            {
                scene_game_clear_all(projectiles, explosions, sounds);
                sounds.sounds.at(DEATH).play();
                current_scene = SCENE_GAME_OVER;
                mainMenu.buttons[BTN_CONTINUE].enabled = false;
                mainMenu.buttons[BTN_SAVE].enabled = false;
                mainMenu.buttons[mainMenu.current].selected = false;
                mainMenu.current = BTN_NEW_GAME;
                mainMenu.buttons[mainMenu.current].selected = true;
                finalScoreText.setString(std::format("Score: {}", maze.score));
                finalScoreText.setPosition({400.0f - finalScoreText.getGlobalBounds().size.x * 0.5f, 250.f});
                int prc = ((float)(mazeData.base_cnt - maze.base_cnt) / (float)mazeData.base_cnt) * 100.0f;
                finalPrcText.setString(std::format("Bases: {}%", prc));
                finalPrcText.setPosition({400.0f - finalPrcText.getGlobalBounds().size.x * 0.5f, 350.f});
            }

            if (maze.base_cnt == 0 && current_scene != SCENE_NO_BASES)
            {
                current_scene = SCENE_NO_BASES;
                mainMenu.buttons[BTN_CONTINUE].enabled = false;
                mainMenu.buttons[BTN_SAVE].enabled = false;
                mainMenu.buttons[mainMenu.current].selected = false;
                mainMenu.current = BTN_NEW_GAME;
                mainMenu.buttons[mainMenu.current].selected = true;
                finalScoreText.setString(std::format("Score: {}", maze.score));
                finalScoreText.setPosition({400.0f - finalScoreText.getGlobalBounds().size.x * 0.5f, 250.f});
                end_game_booms = 500;
            }

            if (current_scene == SCENE_NO_BASES)
            {
                if (sounds.sounds.at(BOOM_BASE).getStatus() != sf::SoundSource::Status::Playing &&
                    explosions.flashing_counter == 0 &&
                    explosions.fragments.empty())
                {
                    scene_game_clear_all(projectiles, explosions, sounds);
                    current_scene = SCENE_WIN;
                }
                else
                {
                    if (end_game_booms > 0)
                    {
                        if (end_game_booms % 3 == 0)
                        {
                            sounds.sounds.at(BOOM).play();
                        }
                        create_explosion(explosions, 
                            {static_cast<float>(std::rand() % WINDOW_W), static_cast<float>(std::rand() % WINDOW_H)}, assets);
                        explosions.fragments.back().check_collision = 0;
                        end_game_booms--;
                    }
                }
            }
        }

        if (current_scene != SCENE_GAME_OVER)
        {
            game_frame++;
        }
    }

    return 0;
}
