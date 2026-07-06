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
#include <set>
#include <memory>
#include <vector>
#include <SFML/System/Vector2.hpp>
#include "ChessUtil.h"

class Piece;
using pieceVector = std::vector<std::shared_ptr<Piece>>;

namespace Chess {

	enum class Direction {
		Up,
		Down,
		Right,
		Left
	};
	constexpr const Chess::Direction directionTable[2][4]{
		{ Chess::Direction::Up, Chess::Direction::Down, Chess::Direction::Right, Chess::Direction::Left },
		{ Chess::Direction::Down, Chess::Direction::Up, Chess::Direction::Left, Chess::Direction::Right }
	 };
	constexpr Chess::Direction getDirection(Chess::Direction direction, Chess::PColor color);

	enum class MoveType {
		All,
		Move,
		Capture
	};

	struct PieceMoves {

		SquareSet moveSet, captureSet;

		PieceMoves() {};
		~PieceMoves() {};

		bool add(const pieceVector& pieceList, Chess::PColor color, sf::Vector2i square, Chess::MoveType type);
		void add(sf::Vector2i square, Chess::MoveType type);
	};

	const std::set<Chess::Direction> allDirections = { Direction::Up, Direction::Down, Direction::Right, Direction::Left };

	class MoveSet {

	public:
		Chess::MoveType type;
		bool initial;

		MoveSet(Chess::MoveType type = Chess::MoveType::All, bool initial = false, const std::set<Chess::Direction>& directions = allDirections) : directions(directions), type(type), initial(initial) {};
		virtual ~MoveSet() {};

		virtual Chess::PieceMoves getSquares(const pieceVector& pieceList, Chess::PColor color, sf::Vector2i piecePos, sf::Vector2u boardSize, bool janggiCannon = false) = 0;
		virtual Chess::SquareSet getSquares(sf::Vector2i piecePos, sf::Vector2u boardSize, Chess::PColor color) = 0;

	protected:
		std::set<Chess::Direction> directions;
	};

	class Adjacent : virtual public MoveSet {

	public:
		Adjacent(bool initial = false, Chess::MoveType type = Chess::MoveType::All, const std::set<Chess::Direction>& directions = allDirections) : MoveSet(type, initial, directions) {};
		~Adjacent() {};

		virtual Chess::PieceMoves getSquares(const pieceVector& pieceList, Chess::PColor color, sf::Vector2i piecePos, sf::Vector2u boardSize, bool janggiCannon = false) override;
		virtual Chess::SquareSet getSquares(sf::Vector2i piecePos, sf::Vector2u boardSize, Chess::PColor color) override;

	};

	class Leaper : virtual public MoveSet {

	public:
		Leaper(int dist, int x, int y, bool initial = false, Chess::MoveType type = Chess::MoveType::All, const std::set<Chess::Direction>& directions = allDirections);
		~Leaper() {};

		virtual Chess::PieceMoves getSquares(const pieceVector& pieceList, Chess::PColor color, sf::Vector2i piecePos, sf::Vector2u boardSize, bool janggiCannon = false) override;
		virtual Chess::SquareSet getSquares(sf::Vector2i piecePos, sf::Vector2u boardSize, Chess::PColor color) override;

	private:
		int x, y, dist;
		std::vector<sf::Vector2i> offsets;

	};

	// 0 goes infinitely
	class Diagonal : virtual public MoveSet {

	public:
		Diagonal(int dist = 0, int step = 1, bool initial = false, bool jumping = false, bool hopping = false, bool grasshopping = false, Chess::MoveType type = Chess::MoveType::All, const std::set<Chess::Direction>& directions = allDirections) :
			MoveSet(type, initial, directions), dist(dist), step(step), jumping(jumping), hopping(hopping), grasshopping(grasshopping) {};
		~Diagonal() {};

		virtual Chess::PieceMoves getSquares(const pieceVector& pieceList, Chess::PColor color, sf::Vector2i piecePos, sf::Vector2u boardSize, bool janggiCannon = false) override;
		virtual Chess::SquareSet getSquares(sf::Vector2i piecePos, sf::Vector2u boardSize, Chess::PColor color) override;

	private:
		int dist, step;
		bool jumping, hopping, grasshopping;
	};

	// 0 goes infinitely
	class Orthogonal : virtual public MoveSet {

	public:
		Orthogonal(int dist = 0, int step = 1, bool initial = false, bool jumping = false, bool hopping = false, bool grasshopping = false, Chess::MoveType type = Chess::MoveType::All, const std::set<Chess::Direction>& directions = allDirections) :
			MoveSet(type, initial, directions), dist(dist), step(step), jumping(jumping), hopping(hopping), grasshopping(grasshopping) {};
		~Orthogonal() {};

		virtual Chess::PieceMoves getSquares(const pieceVector& pieceList, Chess::PColor color, sf::Vector2i piecePos, sf::Vector2u boardSize, bool janggiCannon = false) override;
		virtual Chess::SquareSet getSquares(sf::Vector2i piecePos, sf::Vector2u boardSize, Chess::PColor color) override;

	private:
		int dist, step;
		bool jumping, hopping, grasshopping;
	};

	using MoveSetVector = std::vector<std::shared_ptr<Chess::MoveSet>>;

}