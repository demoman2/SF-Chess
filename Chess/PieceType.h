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
#include <string>
#include <algorithm>

namespace Chess {
	class PieceType {

	public:
		PieceType() {};
		PieceType(const std::string& pieceStr) { pieceSet.insert(pieceStr.begin(), pieceStr.end()); };

		~PieceType() {};

		size_t count() const { return pieceSet.size(); }

		void set(char c) {
			pieceSet.clear();
			if (c != '-') {
				pieceSet.emplace(c);
			}
		}
		void add(char c) {
			if (c != '-') {
				pieceSet.insert(c);
			}
		}
		void remove(char c) { pieceSet.erase(c); };
		bool has(char c) const {
			if (pieceSet.find('*') != pieceSet.end()) {
				return true;
			}
			return pieceSet.find(c) != pieceSet.end();
		}

		void set(const std::string& str) {
			pieceSet.clear();
			if (str != "-") {
				pieceSet.insert(str.begin(), str.end());
			}
		}
		void add(const std::string& str) { 
			if (str != "-") {
				pieceSet.insert(str.begin(), str.end());
			}
		}
		void remove(const std::string& str) { for (auto& c : str) { pieceSet.erase(c); } };
		bool has(const std::string& str) const {
			if (pieceSet.find('*') != pieceSet.end()) {
				return true;
			}
			return std::all_of(str.begin(), str.end(), [&](char c) { return pieceSet.find(c) != pieceSet.end(); });
		}

		void clear() { pieceSet.clear(); }
		bool empty() const { return pieceSet.empty(); }

		const auto& get() const { return pieceSet; }

		auto begin() const { return pieceSet.begin(); }
		auto end() const { return pieceSet.end(); }

		auto cbegin() const { return pieceSet.cbegin(); }
		auto cend() const { return pieceSet.cend(); }

	private:
		std::set<char> pieceSet;

	};
}

