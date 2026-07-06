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
#include <iostream>
#include <thread>
#include <vector>
#include "Board.h"
#include "GUIBoard.h"
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <clip/clip.h>

void addTexture(std::vector<sf::Texture>& textures, const std::string& path, bool smooth = false);

void addImage(std::vector<sf::Image>& images, const std::string& path);

void addSoundBuffer(std::vector<sf::SoundBuffer>& soundBuffers, const std::string& path);

sf::Image makeBoardIcon(const sf::Image& boardSpriteSheet, sf::RenderTexture& renderTexture, const sf::Sprite& iconTexture);

sf::Image makeAIStrengthTexture(const sf::Texture& gradient, sf::Color color);

std::vector<Chess::VariantType> loadVariantTypes(const std::string& path, const std::vector<std::shared_ptr<Chess::Variant>>& variantList);

void makeBoard(std::shared_ptr<Board>& selectedBoard, const BoardSettings& boardSettings, const StockfishSettings& stockfishSettings, const std::vector<std::map<char, sf::Texture>>& pieceTextures,
	const std::map<std::string, std::map<char, sf::Texture>>& variantPieceTextures, sf::Vector2u windowSize,
	const sf::Image& boardSpriteSheet, const sf::Font& textFont, const std::vector<sf::Texture>& boardTextures, const std::vector<sf::SoundBuffer>& soundBuffers, const std::set<std::string>& endgameVariants, std::vector<std::shared_ptr<Board>>& boardList, bool& makingBoard);

std::string timeToStr(const sf::Time& time, std::ostringstream& oss, float multiplier = 1.0f);

std::string floatToStr(float value, std::ostringstream& oss);