#pragma once
#include <algorithm>
#include <cmath>
#include <iostream>
#include <SFML/Graphics/Color.hpp>

struct HSV {
	int h;
	float s, v;
    HSV(int h, float s, float v) : h(h), s(s), v(v) {}
};

HSV RGBtoHSV(const sf::Color& color);
sf::Color HSVtoRGB(HSV hsv);

sf::Color blendColors(const sf::Color& color1, const sf::Color& color2, float blendFactor);

sf::Color setHueSat(const sf::Color& color, HSV hsv);