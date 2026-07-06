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

#include "HSV.h"

HSV RGBtoHSV(const sf::Color& color)
{
	float R = color.r / 255.0f;
	float G = color.g / 255.0f;
	float B = color.b / 255.0f;

	float M = std::max({ R, G, B });
	float m = std::min({ R, G, B });
	float C = M - m; // Chroma

	float H = 0.0f; // Hue
	float S = 0.0f; // Saturation
	float V = 0.0f; // Value

	if (C != 0.0f)
	{
		if (M == R)
			H = std::fmod(((G - B) / C), 6.f);
		else if (M == G)
			H = ((B - R) / C) + 2;
		else if (M == B)
			H = ((R - G) / C) + 4;

		H *= 60;
	}

	if (H < 0.0f)
		H += 360;

	V = M;

	if (V != 0.0f)
		S = C / V;

	return HSV{ (int)std::round(H), S, V };
}

sf::Color HSVtoRGB(HSV hsv)
{
	int hue = hsv.h;
	float sat = hsv.s;
	float val = hsv.v;

	hue %= 360;
	while (hue < 0) hue += 360;

	if (sat < 0.0f) sat = 0.0f;
	if (sat > 1.0f) sat = 1.0f;

	if (val < 0.0f) val = 0.0f;
	if (val > 1.0f) val = 1.0f;

	int h = hue / 60;
	float f = float(hue) / 60 - h;
	float p = val * (1.0f - sat);
	float q = val * (1.0f - sat * f);
	float t = val * (1.0f - sat * (1 - f));

	switch (h)
	{
	default:
	case 0:
	case 6: return sf::Color((int)std::round(val * 255), (int)std::round(t * 255), (int)std::round(p * 255));
	case 1: return sf::Color((int)std::round(q * 255), (int)std::round(val * 255), (int)std::round(p * 255));
	case 2: return sf::Color((int)std::round(p * 255), (int)std::round(val * 255), (int)std::round(t * 255));
	case 3: return sf::Color((int)std::round(p * 255), (int)std::round(q * 255), (int)std::round(val * 255));
	case 4: return sf::Color((int)std::round(t * 255), (int)std::round(p * 255), (int)std::round(val * 255));
	case 5: return sf::Color((int)std::round(val * 255), (int)std::round(p * 255), (int)std::round(q * 255));
	}
	return sf::Color();
}

sf::Color blendColors(const sf::Color& color1, const sf::Color& color2, float blendFactor)
{
	blendFactor = std::clamp(blendFactor, 0.0f, 1.0f);

	uint8_t r = static_cast<uint8_t>(color1.r * (1 - blendFactor) + color2.r * blendFactor);
	uint8_t g = static_cast<uint8_t>(color1.g * (1 - blendFactor) + color2.g * blendFactor);
	uint8_t b = static_cast<uint8_t>(color1.b * (1 - blendFactor) + color2.b * blendFactor);
	uint8_t a = static_cast<uint8_t>(color1.a * (1 - blendFactor) + color2.a * blendFactor);

	return sf::Color(r, g, b, a);
}

sf::Color setHueSat(const sf::Color& color, HSV hsv)
{
	HSV newHSV = RGBtoHSV(color);
	newHSV.h = hsv.h;
	newHSV.s *= (1.364f * hsv.s) + 0.509f;
	newHSV.v *= std::clamp((1.5385f * hsv.v), 0.9f, 1.1f);
	return HSVtoRGB(newHSV);
}