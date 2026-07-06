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
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Main.hpp>
#include <SFML/Window.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include "Variant.h"

class SVGPiece {

private:
	tgui::Picture::Ptr pieceImage;

public:
	SVGPiece(const tgui::Texture& texture, int x, int y, sf::Vector2f boardOffset, sf::Vector2f boardSize, sf::Vector2u boardSquares, sf::Vector2f boardMultiplier, bool reversed);
	void addGUI(tgui::Gui& gui);
};

class GUIBoard {
	
private:
	tgui::Gui* gui;
	std::vector<SVGPiece> previewPieces;
	sf::Vector2f boardOffset, boardSize, startingOffset, boardMultiplier;
	float boardScale;
public:
	GUIBoard(const tgui::Picture::Ptr& boardPicture, tgui::Gui& gui);
	void setPieces(const std::string& fen, const Chess::Variant& variant, const std::map<char, tgui::Texture>& pieceTextures, bool reversed = false);
	void setBoard(const tgui::Picture::Ptr& boardPicture, sf::Vector2u boardSquares);
};

std::vector<SVGPiece> generatePositionFromFENID(const std::string& code, const Chess::Variant& variant, const std::map<char, tgui::Texture>& pieceTextures, sf::Vector2f boardOffset, sf::Vector2f boardSize, sf::Vector2f boardMultiplier, bool reversed = false);