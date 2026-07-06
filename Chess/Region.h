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
#include <algorithm>
#include <SFML/System/Vector2.hpp>
#include <regex>
#include "ChessUtil.h"

namespace Chess {

	class Region {

	public:
		std::vector<std::string> regions;
		static const std::regex filePattern, rankPattern, squarePattern;

		Region();
		Region(const std::vector<std::string>& regions);
		Region(const std::string& regionStr);
		void load(const std::string& regionStr);

		~Region() {}

		bool contains(sf::Vector2i sq) const;
		bool empty() const;
		Chess::SquareSet getSquares(sf::Vector2u boardSize) const;

		Region operator+(const Region& other);
		Region operator-(const Region& other);
		bool operator==(const Region& other) { return regions == other.regions; };
		bool operator!=(const Region& other) { return regions != other.regions; };

	};
}
