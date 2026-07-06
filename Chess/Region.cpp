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

#include "Region.h"
#include <iostream>

const std::regex Chess::Region::rankPattern("\\*\\d+");
const std::regex Chess::Region::filePattern("[a-z]\\*");
const std::regex Chess::Region::squarePattern("[a-z]\\d+");

Chess::Region::Region() {}

Chess::Region::Region(const std::vector<std::string>& regions) : regions(regions) {}

Chess::Region::Region(const std::string& regionStr)
{
	load(regionStr);
}

void Chess::Region::load(const std::string& regionStr)
{
	regions = split_if_not(regionStr, ' ', '-');
}

bool Chess::Region::contains(sf::Vector2i sq) const
{
	for (const auto& str : regions) {
		if (str == "*") {
			return true;
		}
		else if (std::regex_match(str, rankPattern)) {
			if (sq.y == std::stoi(str.substr(1))) {
				return true;
			}
		}
		else if (std::regex_match(str, filePattern)) {
			if (sq.x == convertChartoX(str.front()) + 1) {
				return true;
			}
		}
		else if (std::regex_match(str, squarePattern)) {
			if (sq == convertChessNotationtoPosition(str)) {
				return true;
			}
		}
	}
	return false;
}

bool Chess::Region::empty() const
{
	return regions.empty();
}

Chess::SquareSet Chess::Region::getSquares(sf::Vector2u boardSize) const
{
	Chess::SquareSet rgSquares;
	for (const auto& rg : regions) {
		if (rg == "-") { continue; }

		if (rg == "*") {
			for (int x = 1; x <= (int)boardSize.x; x++) {
				for (int y = 1; y <= (int)boardSize.y; y++) {
					rgSquares.emplace(x, y);
				}
			}
			return rgSquares;
		}
		else if (std::regex_match(rg, rankPattern)) {
			int y = std::stoi(rg.substr(1));
			for (int x = 1; x <= (int)boardSize.x; x++) {
				rgSquares.emplace(x, y);
			}
		}
		else if (std::regex_match(rg, filePattern)) {
			int x = convertChartoX(rg.front()) + 1;
			for (int y = 1; y <= (int)boardSize.y; y++) {
				rgSquares.emplace(x, y);
			}
		}
		else if (std::regex_match(rg, squarePattern)) {
			rgSquares.emplace(convertChessNotationtoPosition(rg));
		}
	}
	return rgSquares;
}

Chess::Region Chess::Region::operator+(const Region& other)
{
	auto v = regions;
	for (auto& sq : other.regions) {
		if (std::find(v.begin(), v.end(), sq) != v.end()) {
			v.push_back(sq);
		}
	}
	return Region(v);
}

Chess::Region Chess::Region::operator-(const Region& other)
{
	Region r(regions);
	for (auto& sq : other.regions) {
		for (auto it = r.regions.begin(); it != r.regions.end(); it++) {
			if (*it == sq) {
				it = r.regions.erase(it);
			}
		}
	}
	return r;
}