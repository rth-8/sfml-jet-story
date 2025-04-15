#ifndef ZX_COLORS_H
#define ZX_COLORS_H

#include <vector>

const std::vector<sf::Color> zx_colors {
    // normal: 0 - 7
    sf::Color(0,   0,   0),     // black    0
    sf::Color(0,   0,   204),   // blue     1
    sf::Color(204, 0,   0),     // red      2
    sf::Color(204, 0,   204),   // magenta  3
    sf::Color(0,   204, 0),     // green    4
    sf::Color(0,   204, 204),   // cyan     5
    sf::Color(204, 204, 0),     // yellow   6
    sf::Color(204, 204, 204),   // white    7
    // bright: 8 - 15
    sf::Color(0,   0,   0),     // black    8
    sf::Color(0,   0,   255),   // blue     9
    sf::Color(255, 0,   0),     // red      10
    sf::Color(255, 0,   255),   // magenta  11
    sf::Color(0,   255, 0),     // green    12
    sf::Color(0,   255, 255),   // cyan     13
    sf::Color(255, 255, 0),     // yellow   14
    sf::Color(255, 255, 255),   // white    15
};

const std::array item_colors_loop {9, 12, 13, 14, 15, 14, 13};

#endif
