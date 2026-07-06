/*
    SF Chess, a Chess GUI which supports many chess variants
    Copyright (C) 2026  demoman2 (https://github.com/demoman2)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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