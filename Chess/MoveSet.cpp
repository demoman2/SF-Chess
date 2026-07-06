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

#include "MoveSet.h"
#include "ChessUtil.h"
#include "Piece.h"
#include <iostream>

Chess::SquareSet Chess::Adjacent::getSquares(sf::Vector2i piecePos, sf::Vector2u boardSize, Chess::PColor color)
{
	Chess::SquareSet pSquares;
	auto addSquare = [&](sf::Vector2i offset) {
		sf::Vector2i newSquare = { piecePos.x + offset.x, piecePos.y + offset.y };
		if (Chess::isValidSquare(newSquare, boardSize)) {
			pSquares.insert(newSquare);
		}
	};

	if (directions.find(getDirection(Chess::Direction::Up, color)) != directions.end()) {
		addSquare({ 0, 1 });
		addSquare({ 1, 1 });
		addSquare({ -1, 1 });
	}
	if (directions.find(getDirection(Chess::Direction::Down, color)) != directions.end()) {
		addSquare({ 0, -1 });
		addSquare({ 1, -1 });
		addSquare({ -1, -1 });
	}
	if (directions.find(getDirection(Chess::Direction::Right, color)) != directions.end()) {
		addSquare({ 1, 1 });
		addSquare({ 1, -1 });
		addSquare({ 1, 0 });
	}
	if (directions.find(getDirection(Chess::Direction::Left, color)) != directions.end()) {
		addSquare({ -1, 1 });
		addSquare({ -1, -1 });
		addSquare({ -1, 0 });
	}
	return pSquares;
}

Chess::PieceMoves Chess::Adjacent::getSquares(const pieceVector& pieceList, Chess::PColor color, sf::Vector2i piecePos, sf::Vector2u boardSize, bool janggiCannon)
{
	Chess::PieceMoves pSquares;
	auto addSquare = [&](sf::Vector2i offset) {
		sf::Vector2i newSquare = { piecePos.x + offset.x, piecePos.y + offset.y };
		if (Chess::isValidSquare(newSquare, boardSize)) {
			pSquares.add(pieceList, color, newSquare, type);
		}
	};

	if (directions.find(getDirection(Chess::Direction::Up, color)) != directions.end() || directions.empty()) {
		addSquare({ 0, 1 });
		addSquare({ 1, 1 });
		addSquare({ -1, 1 });
	}
	if (directions.find(getDirection(Chess::Direction::Down, color)) != directions.end() || directions.empty()) {
		addSquare({ 0, -1 });
		addSquare({ 1, -1 });
		addSquare({ -1, -1 });
	}
	if (directions.find(getDirection(Chess::Direction::Right, color)) != directions.end() || directions.empty()) {
		addSquare({ 1, 1 });
		addSquare({ 1, -1 });
		addSquare({ 1, 0 });
	}
	if (directions.find(getDirection(Chess::Direction::Left, color)) != directions.end() || directions.empty()) {
		addSquare({ -1, 1 });
		addSquare({ -1, -1 });
		addSquare({ -1, 0 });
	}
	return pSquares;
}

Chess::SquareSet Chess::Leaper::getSquares(sf::Vector2i piecePos, sf::Vector2u boardSize, Chess::PColor color)
{
	offsets.clear();
	if (directions.find(getDirection(Chess::Direction::Up, color)) != directions.end() || directions.empty()) {
		offsets.emplace_back(-x, y);
		offsets.emplace_back(x, y);
	}
	if (directions.find(getDirection(Chess::Direction::Down, color)) != directions.end() || directions.empty()) {
		offsets.emplace_back(-x, -y);
		offsets.emplace_back(x, -y);
	}
	if (directions.find(getDirection(Chess::Direction::Right, color)) != directions.end() || directions.empty()) {
		offsets.emplace_back(y, x);
		offsets.emplace_back(y, -x);
	}
	if (directions.find(getDirection(Chess::Direction::Left, color)) != directions.end() || directions.empty()) {
		offsets.emplace_back(-y, x);
		offsets.emplace_back(-y, -x);
	}
	Chess::SquareSet pSquares;
	for (const auto& offset : offsets) {
		for (int s = 1; (dist == 0) ? true : s <= dist; s++) {
			sf::Vector2i newPos = piecePos +  sf::Vector2i{ offset.x * s, offset.y * s };
			if (Chess::isValidSquare(newPos, boardSize)) {
				pSquares.insert(newPos);
			}
			else { break; }
		}
	}
	return pSquares;
}

Chess::PieceMoves Chess::Leaper::getSquares(const pieceVector& pieceList, Chess::PColor color, sf::Vector2i piecePos, sf::Vector2u boardSize, bool janggiCannon)
{
	offsets.clear();
	if (directions.find(getDirection(Chess::Direction::Up, color)) != directions.end() || directions.empty()) {
		offsets.emplace_back(-x, y);
		offsets.emplace_back(x, y);
	}
	if (directions.find(getDirection(Chess::Direction::Down, color)) != directions.end() || directions.empty()) {
		offsets.emplace_back(-x, -y);
		offsets.emplace_back(x, -y);
	}
	if (directions.find(getDirection(Chess::Direction::Right, color)) != directions.end() || directions.empty()) {
		offsets.emplace_back(y, x);
		offsets.emplace_back(y, -x);
	}
	if (directions.find(getDirection(Chess::Direction::Left, color)) != directions.end() || directions.empty()) {
		offsets.emplace_back(-y, x);
		offsets.emplace_back(-y, -x);
	}

	Chess::PieceMoves pSquares;
	for (const auto& offset : offsets) {
		for (int s = 1; (dist == 0) ? true : s <= dist; s++) {
			sf::Vector2i newPos = piecePos + sf::Vector2i{ offset.x * s, offset.y * s };
			if (Chess::isValidSquare(newPos, boardSize)) {
				if (pSquares.add(pieceList, color, newPos, type)) { break; }
			}
			else { break; }
		}
	}
	return pSquares;
}

Chess::Leaper::Leaper(int dist, int x, int y, bool initial, Chess::MoveType type, const std::set<Chess::Direction>& directions) : MoveSet(type, initial, directions), x(x), y(y), dist(dist)
{
}

Chess::SquareSet Chess::Diagonal::getSquares(sf::Vector2i piecePos, sf::Vector2u boardSize, Chess::PColor color)
{

	Chess::SquareSet pSquares;
	unsigned d = (dist == 0) ? std::max(boardSize.x, boardSize.y) : (dist * step);
	// Top Right Diag
	if (directions.empty() || ((directions.find(getDirection(Chess::Direction::Up, color)) != directions.end()
		&& directions.find(getDirection(Chess::Direction::Left, color)) == directions.end()) || directions.find(getDirection(Chess::Direction::Right, color)) != directions.end())) {
		for (unsigned i = step; i <= d; i += step) {
			int newX = piecePos.x + i, newY = piecePos.y + i;
			if (!isValidSquare({ newX, newY }, boardSize)) { break; }
			pSquares.emplace(newX, newY);
		}
	}
	// Top Left Diag
	if (directions.empty() || ((directions.find(getDirection(Chess::Direction::Up, color)) != directions.end()
		&& directions.find(getDirection(Chess::Direction::Left, color)) == directions.end()) || directions.find(getDirection(Chess::Direction::Left, color)) != directions.end())) {
		for (unsigned i = step; i <= d; i += step) {
			int newX = piecePos.x - i, newY = piecePos.y + i;
			if (!isValidSquare({ newX, newY }, boardSize)) { break; }
			pSquares.emplace(newX, newY);
		}
	}
	// Bottom Right Diag
	if (directions.empty() || ((directions.find(getDirection(Chess::Direction::Down, color)) != directions.end()
		&& directions.find(getDirection(Chess::Direction::Left, color)) == directions.end()) || directions.find(getDirection(Chess::Direction::Right, color)) != directions.end())) {
		for (unsigned i = step; i <= d; i += step) {
			int newX = piecePos.x + i, newY = piecePos.y - i;
			if (!isValidSquare({ newX, newY }, boardSize)) { break; }
			pSquares.emplace(newX, newY);
		}
	}
	// Bottom Left Diag
	if (directions.empty() || ((directions.find(getDirection(Chess::Direction::Down, color)) != directions.end()
		&& directions.find(getDirection(Chess::Direction::Right, color)) == directions.end()) || directions.find(getDirection(Chess::Direction::Left, color)) != directions.end())) {
		for (unsigned i = step; i <= d; i += step) {
			int newX = piecePos.x - i, newY = piecePos.y - i;
			if (!isValidSquare({ newX, newY }, boardSize)) { break; }
			pSquares.emplace(newX, newY);
		}
	}
	return pSquares;
}

Chess::PieceMoves Chess::Diagonal::getSquares(const pieceVector& pieceList, Chess::PColor color, sf::Vector2i piecePos, sf::Vector2u boardSize, bool janggiCannon)
{
	Chess::PieceMoves pSquares;
	unsigned d = (dist == 0) ? std::max(boardSize.x, boardSize.y) : (dist * step);
	// Top Right Diag
	if (directions.empty() || ((directions.find(getDirection(Chess::Direction::Up, color)) != directions.end()
		&& directions.find(getDirection(Chess::Direction::Left, color)) == directions.end()) || directions.find(getDirection(Chess::Direction::Right, color)) != directions.end())) {
		bool piece = false;
		for (unsigned i = 1; i <= d; i++) {
			bool hasMove = i % step == 0;
			int newX = piecePos.x + i, newY = piecePos.y + i;
			if (!Chess::isValidSquare({ newX, newY }, boardSize)) { break; }
			if (!hopping && !grasshopping) {
				if (hasMove) {
					pSquares.add(pieceList, color, { newX, newY }, type);
				}
				if (!jumping && Chess::getPieceFromPosition({ newX, newY }, pieceList)) { break; }
			}
			else if (hasMove) {
				auto currentPiece = Chess::getPieceFromPosition(piecePos, pieceList);
				auto newPiece = Chess::getPieceFromPosition({ newX, newY }, pieceList);
				if (janggiCannon && newPiece && newPiece->id == currentPiece->id) {
					break;
				}
				if (!piece && newPiece) {
					piece = true;
					continue;
				}
				if (grasshopping) {
					if (piece) {
						pSquares.add(pieceList, color, { newX, newY }, type);
						break;
					}
				}
				else {
					if (piece && pSquares.add(pieceList, color, { newX, newY }, type)) { break; }
				}
			}
		}
	}
	// Top Left Diag
	if (directions.empty() || ((directions.find(getDirection(Chess::Direction::Up, color)) != directions.end()
		&& directions.find(getDirection(Chess::Direction::Left, color)) == directions.end()) || directions.find(getDirection(Chess::Direction::Left, color)) != directions.end())) {
		bool piece = false;
		for (unsigned i = 1; i <= d; i++) {
			bool hasMove = i % step == 0;
			int newX = piecePos.x - i, newY = piecePos.y + i;
			if (!Chess::isValidSquare({ newX, newY }, boardSize)) { break; }
			if (!hopping && !grasshopping) {
				if (hasMove) {
					pSquares.add(pieceList, color, { newX, newY }, type);
				}
				if (!jumping && Chess::getPieceFromPosition({ newX, newY }, pieceList)) { break; }
			}
			else if (hasMove) {
				auto currentPiece = Chess::getPieceFromPosition(piecePos, pieceList);
				auto newPiece = Chess::getPieceFromPosition({ newX, newY }, pieceList);
				if (janggiCannon && newPiece && newPiece->id == currentPiece->id) {
					break;
				}
				if (!piece && newPiece) {
					piece = true;
					continue;
				}
				if (grasshopping) {
					if (piece) {
						pSquares.add(pieceList, color, { newX, newY }, type);
						break;
					}
				}
				else {
					if (piece && pSquares.add(pieceList, color, { newX, newY }, type)) { break; }
				}
			}
		}
	}
	// Bottom Right Diag
	if (directions.empty() || ((directions.find(getDirection(Chess::Direction::Down, color)) != directions.end()
		&& directions.find(getDirection(Chess::Direction::Left, color)) == directions.end()) || directions.find(getDirection(Chess::Direction::Right, color)) != directions.end())) {
		bool piece = false;
		for (unsigned i = 1; i <= d; i++) {
			bool hasMove = i % step == 0;
			int newX = piecePos.x + i, newY = piecePos.y - i;
			if (!Chess::isValidSquare({ newX, newY }, boardSize)) { break; }
			if (!hopping && !grasshopping) {
				if (hasMove) {
					pSquares.add(pieceList, color, { newX, newY }, type);
				}
				if (!jumping && Chess::getPieceFromPosition({ newX, newY }, pieceList)) { break; }
			}
			else if (hasMove) {
				auto currentPiece = Chess::getPieceFromPosition(piecePos, pieceList);
				auto newPiece = Chess::getPieceFromPosition({ newX, newY }, pieceList);
				if (janggiCannon && newPiece && newPiece->id == currentPiece->id) {
					break;
				}
				if (!piece && newPiece) {
					piece = true;
					continue;
				}
				if (grasshopping) {
					if (piece) {
						pSquares.add(pieceList, color, { newX, newY }, type);
						break;
					}
				}
				else {
					if (piece && pSquares.add(pieceList, color, { newX, newY }, type)) { break; }
				}
			}
		}
	}
	// Bottom Left Diag
	if (directions.empty() || ((directions.find(getDirection(Chess::Direction::Down, color)) != directions.end()
		&& directions.find(getDirection(Chess::Direction::Right, color)) == directions.end()) || directions.find(getDirection(Chess::Direction::Left, color)) != directions.end())) {
		bool piece = false;
		for (unsigned i = 1; i <= d; i++) {
			bool hasMove = i % step == 0;
			int newX = piecePos.x - i, newY = piecePos.y - i;
			if (!Chess::isValidSquare({ newX, newY }, boardSize)) { break; }
			if (!hopping && !grasshopping) {
				if (hasMove) {
					pSquares.add(pieceList, color, { newX, newY }, type);
				}
				if (!jumping && Chess::getPieceFromPosition({ newX, newY }, pieceList)) { break; }
			}
			else if (hasMove) {
				auto currentPiece = Chess::getPieceFromPosition(piecePos, pieceList);
				auto newPiece = Chess::getPieceFromPosition({ newX, newY }, pieceList);
				if (janggiCannon && newPiece && newPiece->id == currentPiece->id) {
					break;
				}
				if (!piece && newPiece) {
					piece = true;
					continue;
				}
				if (grasshopping) {
					if (piece) {
						pSquares.add(pieceList, color, { newX, newY }, type);
						break;
					}
				}
				else {
					if (piece && pSquares.add(pieceList, color, { newX, newY }, type)) { break; }
				}
			}
		}
	}
	return pSquares;
}

Chess::SquareSet Chess::Orthogonal::getSquares(sf::Vector2i piecePos, sf::Vector2u boardSize, Chess::PColor color)
{

	Chess::SquareSet pSquares;
	unsigned d = (dist == 0) ? std::max(boardSize.x, boardSize.y) : (dist * step);
	// Up
	if (directions.empty() || directions.find(getDirection(Chess::Direction::Up, color)) != directions.end()) {
		for (unsigned i = step; i <= d; i += step) {
			int newX = piecePos.x, newY = piecePos.y + i;
			if (newY > boardSize.y) { break; }
			pSquares.emplace(newX, newY);
		}
	}
	// Down
	if (directions.empty() || directions.find(getDirection(Chess::Direction::Down, color)) != directions.end()) {
		for (unsigned i = step; i <= d; i += step) {
			int newX = piecePos.x, newY = piecePos.y - i;
			if (newY < 1) { break; }
			pSquares.emplace(newX, newY);
		}
	}
	// Right
	if (directions.empty() || directions.find(getDirection(Chess::Direction::Right, color)) != directions.end()) {
		for (unsigned i = step; i <= d; i += step) {
			int newX = piecePos.x + i, newY = piecePos.y;
			if (newX > boardSize.x) { break; }
			pSquares.emplace(newX, newY);
		}
	}
	// Left
	if (directions.empty() || directions.find(getDirection(Chess::Direction::Left, color)) != directions.end()) {
		for (unsigned i = step; i <= d; i += step) {
			int newX = piecePos.x - i, newY = piecePos.y;
			if (newX < 1) { break; }
			pSquares.emplace(newX, newY);
		}
	}
	return pSquares;
}

Chess::PieceMoves Chess::Orthogonal::getSquares(const pieceVector& pieceList, Chess::PColor color, sf::Vector2i piecePos, sf::Vector2u boardSize, bool janggiCannon)
{
	Chess::PieceMoves pSquares;
	unsigned d = (dist == 0) ? std::max(boardSize.x, boardSize.y) : (dist * step);
	// Up
	if (directions.empty() || directions.find(getDirection(Chess::Direction::Up, color)) != directions.end()) {
		bool piece = false;
		for (unsigned i = 1; i <= d; i++) {
			bool hasMove = i % step == 0;
			int newX = piecePos.x, newY = piecePos.y + i;
			if (!Chess::isValidSquare({ newX, newY }, boardSize)) { break; }
			if (!hopping && !grasshopping) {
				if (hasMove) {
					pSquares.add(pieceList, color, { newX, newY }, type);
				}
				if (!jumping && Chess::getPieceFromPosition({ newX, newY }, pieceList)) { break; }
			}
			else if (hasMove) {
				auto currentPiece = Chess::getPieceFromPosition(piecePos, pieceList);
				auto newPiece = Chess::getPieceFromPosition({ newX, newY }, pieceList);
				if (janggiCannon && newPiece && newPiece->id == currentPiece->id) {
					break;
				}
				if (!piece && newPiece) {
					piece = true;
					continue;
				}
				if (grasshopping) {
					if (piece) {
						pSquares.add(pieceList, color, { newX, newY }, type);
						break;
					}
				}
				else {
					if (piece && pSquares.add(pieceList, color, { newX, newY }, type)) { break; }
				}				
			}
		}
	}
	// Down
	if (directions.empty() || directions.find(getDirection(Chess::Direction::Down, color)) != directions.end()) {
		bool piece = false;
		for (unsigned i = 1; i <= d; i++) {
			bool hasMove = i % step == 0;
			int newX = piecePos.x, newY = piecePos.y - i;
			if (!Chess::isValidSquare({ newX, newY }, boardSize)) { break; }
			if (!hopping && !grasshopping) {
				if (hasMove) {
					pSquares.add(pieceList, color, { newX, newY }, type);
				}
				if (!jumping && Chess::getPieceFromPosition({ newX, newY }, pieceList)) { break; }
			}
			else if (hasMove) {
				auto currentPiece = Chess::getPieceFromPosition(piecePos, pieceList);
				auto newPiece = Chess::getPieceFromPosition({ newX, newY }, pieceList);
				if (janggiCannon && newPiece && newPiece->id == currentPiece->id) {
					break;
				}
				if (!piece && newPiece) {
					piece = true;
					continue;
				}
				if (grasshopping) {
					if (piece) {
						pSquares.add(pieceList, color, { newX, newY }, type);
						break;
					}
				}
				else {
					if (piece && pSquares.add(pieceList, color, { newX, newY }, type)) { break; }
				}				
			}
		}
	}
	// Right
	if (directions.empty() || directions.find(getDirection(Chess::Direction::Right, color)) != directions.end()) {
		bool piece = false;
		for (unsigned i = 1; i <= d; i++) {
			bool hasMove = i % step == 0;
			int newX = piecePos.x + i, newY = piecePos.y;
			if (!Chess::isValidSquare({ newX, newY }, boardSize)) { break; }
			if (!hopping && !grasshopping) {
				if (hasMove) {
					pSquares.add(pieceList, color, { newX, newY }, type);
				}
				if (!jumping && Chess::getPieceFromPosition({ newX, newY }, pieceList)) { break; }
			}
			else if (hasMove) {
				auto currentPiece = Chess::getPieceFromPosition(piecePos, pieceList);
				auto newPiece = Chess::getPieceFromPosition({ newX, newY }, pieceList);
				if (janggiCannon && newPiece && newPiece->id == currentPiece->id) {
					break;
				}
				if (!piece && newPiece) {
					piece = true;
					continue;
				}
				if (grasshopping) {
					if (piece) {
						pSquares.add(pieceList, color, { newX, newY }, type);
						break;
					}
				}
				else {
					if (piece && pSquares.add(pieceList, color, { newX, newY }, type)) { break; }
				}
			}
		}
	}
	// Left
	if (directions.empty() || directions.find(getDirection(Chess::Direction::Left, color)) != directions.end()) {
		bool piece = false;
		for (unsigned i = 1; i <= d; i++) {
			bool hasMove = i % step == 0;
			int newX = piecePos.x - i, newY = piecePos.y;
			if (!Chess::isValidSquare({ newX, newY }, boardSize)) { break; }
			if (!hopping && !grasshopping) {
				if (hasMove) {
					pSquares.add(pieceList, color, { newX, newY }, type);
				}
				if (!jumping && Chess::getPieceFromPosition({ newX, newY }, pieceList)) { break; }
			}
			else if (hasMove) {
				auto currentPiece = Chess::getPieceFromPosition(piecePos, pieceList);
				auto newPiece = Chess::getPieceFromPosition({ newX, newY }, pieceList);
				if (janggiCannon && newPiece && newPiece->id == currentPiece->id) {
					break;
				}
				if (!piece && newPiece) {
					piece = true;
					continue;
				}
				if (grasshopping) {
					if (piece) {
						pSquares.add(pieceList, color, { newX, newY }, type);
						break;
					}
				}
				else {
					if (piece && pSquares.add(pieceList, color, { newX, newY }, type)) { break; }
				}
			}
		}
	}
	return pSquares;
}

bool Chess::PieceMoves::add(const pieceVector& pieceList, Chess::PColor color, sf::Vector2i square, Chess::MoveType type)
{
	if (Chess::isPieceAt(square, pieceList)) {
		if (Chess::getPieceFromPosition(square, pieceList)->color != color && (type == MoveType::Capture || type == MoveType::All)) {
			captureSet.insert(square);
		}
		return true;
	}
	else if (type == MoveType::Move || type == MoveType::All) {
		moveSet.insert(square);
	}
	return false;
}

void Chess::PieceMoves::add(sf::Vector2i square, Chess::MoveType type)
{
	if (type == MoveType::Move || type == MoveType::All) {
		moveSet.insert(square);
	}
	if (type == MoveType::Capture || type == MoveType::All) {
		captureSet.insert(square);
	}
}

constexpr Chess::Direction Chess::getDirection(Chess::Direction direction, Chess::PColor color)
{
	return directionTable[static_cast<size_t>(color)][static_cast<size_t>(direction)];
}
