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

#include "ChessUtil.h"
#include "Piece.h"
#include "Variant.h"
#include "MoveSet.h"
#include <iostream>

sf::Vector2i Chess::convertChessNotationtoPosition(const std::string& notation)
{
	if (notation.size() < 2) { return sf::Vector2i{}; }
	int x = (notation.at(0) - 'a') + 1;
	int y = std::stoi(notation.substr(1));
	return { x, y };
}

std::string Chess::convertPositiontoNotation(sf::Vector2i position)
{
	std::string notation;
	notation += char((position.x + 'a') - 1);
	notation += std::to_string(position.y);
	return notation;
}

int Chess::convertXtoChar(int x)
{
	return convertPositiontoNotation({ x + 1, 0 }).front();
}

int Chess::convertChartoX(char c)
{
	return c - 'a';
}

sf::Vector2i Chess::reversePositon(sf::Vector2i position, sf::Vector2u boardSize)
{
	return { Chess::reverseY(position.x, boardSize),Chess::reverseY(position.y, boardSize) };
}

sf::Vector2f Chess::reversePosition(sf::Vector2f position, sf::Vector2f boardSize)
{
	return (boardSize - position);
}

sf::Vector2f Chess::getGlobalPosition(sf::Vector2i position, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, sf::Vector2u boardSquares, bool reversed)
{
	sf::Vector2f r{ ((float(position.x) - 0.5f) * boardMultiplier), (Chess::reverseY(float(position.y), boardSquares) - 0.5f) * boardMultiplier };
	if (reversed) { return Chess::reversePosition(r, boardSize) + boardOffset; }
	return r + boardOffset;
}

sf::Vector2f Chess::getGlobalPosition(sf::Vector2i position, sf::Vector2f boardOffset, sf::Vector2f boardSize, sf::Vector2f boardMultiplier, sf::Vector2u boardSquares, bool reversed)
{
	sf::Vector2f r{ ((float(position.x) - 0.5f) * boardMultiplier.x), (Chess::reverseY(float(position.y), boardSquares) - 0.5f) * boardMultiplier.y };
	if (reversed) { return Chess::reversePosition(r, boardSize) + boardOffset; }
	return r + boardOffset;
}


sf::Vector2f Chess::getGlobalPosition(sf::Vector2f position, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, sf::Vector2u boardSquares, bool reversed)
{
	sf::Vector2f r{ ((position.x - 0.5f) * boardMultiplier), (Chess::reverseY(position.y, boardSquares) - 0.5f) * boardMultiplier };
	if (reversed) { return Chess::reversePosition(r, boardSize) + boardOffset; }
	return r + boardOffset;
}

sf::Vector2i Chess::getLocalPosition(sf::Vector2f position, sf::Vector2f boardOffset, float boardMultiplier, sf::Vector2u boardSquares, bool reversed)
{
	sf::Vector2f v = { std::ceil((position.x - boardOffset.x) / boardMultiplier), Chess::reverseY(std::ceil((position.y - boardOffset.y) / boardMultiplier), boardSquares) };
	sf::Vector2i d = (sf::Vector2i)v;
	if (reversed) { return { Chess::reverseY(d.x, boardSquares), Chess::reverseY(d.y, boardSquares) }; }
	return d;
}

bool Chess::isPieceAt(sf::Vector2i position, const pieceVector& pieces)
{
	return getPieceFromPosition(position, pieces) != nullptr;
}

std::shared_ptr<Piece> Chess::getPieceFromPosition(sf::Vector2i position, const pieceVector& pieces)
{
	for (auto& piece : pieces) {
		if (piece->getLocalPos() == position) {
			return piece;
		}
	}
	return nullptr;
}

Chess::MoveSetVector Chess::getMoveSets(const std::string& moveStr)
{
	std::string allPieces = { "BNRQKWFDCZAHG" };

	Chess::MoveSetVector moveSets;
	std::set<Chess::Direction> currentDirs;
	Chess::MoveType moveType = Chess::MoveType::All;
	bool hopping = false, grasshopping = false, nonjumping = false, initial = false;
	char previous = ' ', currentPiece = ' ';

	auto addPiece = [&](char piece) {
		if (piece == 'B') {
			moveSets.emplace_back(std::make_shared<Chess::Diagonal>(0, 1, initial, false, hopping, grasshopping, moveType, currentDirs));
		}
		else if (piece == 'R') {
			moveSets.emplace_back(std::make_shared<Chess::Orthogonal>(0, 1, initial, false, hopping, grasshopping, moveType, currentDirs));
		}
		else if (piece == 'F') {
			moveSets.emplace_back(std::make_shared<Chess::Diagonal>(1, 1, initial, false, hopping, grasshopping, moveType, currentDirs));
		}
		else if (piece == 'A') {
			moveSets.emplace_back(std::make_shared<Chess::Diagonal>(1, 2, initial, nonjumping ? false : true, hopping, grasshopping, moveType, currentDirs));
		}
		else if (piece == 'Q') {
			moveSets.emplace_back(std::make_shared<Chess::Diagonal>(0, 1, initial, false, hopping, grasshopping, moveType, currentDirs));
			moveSets.emplace_back(std::make_shared<Chess::Orthogonal>(0, 1, initial, false, hopping, grasshopping, moveType, currentDirs));
		}
		else if (piece == 'W') {
			moveSets.emplace_back(std::make_shared<Chess::Orthogonal>(1, 1, initial, false, hopping, grasshopping, moveType, currentDirs));
		}
		else if (piece == 'D') {
			moveSets.emplace_back(std::make_shared<Chess::Orthogonal>(1, 2, initial, nonjumping ? false : true, hopping, grasshopping, moveType, currentDirs));
		}
		else if (piece == 'N') {
			moveSets.emplace_back(std::make_shared<Chess::Leaper>(1, 1, 2, initial, moveType, currentDirs));
		}
		else if (piece == 'C') {
			moveSets.emplace_back(std::make_shared<Chess::Leaper>(1, 1, 3, initial, moveType, currentDirs));
		}
		else if (piece == 'Z') {
			moveSets.emplace_back(std::make_shared<Chess::Leaper>(1, 2, 3, initial, moveType, currentDirs));
		}
		else if (piece == 'K') {
			moveSets.emplace_back(std::make_shared<Chess::Adjacent>(initial, moveType, currentDirs));
		}
		else if (piece == 'H') {
			moveSets.emplace_back(std::make_shared<Chess::Orthogonal>(1, 3, initial, nonjumping ? false : true, hopping, grasshopping, moveType, currentDirs));
		}
		else if (piece == 'G') {
			moveSets.emplace_back(std::make_shared<Chess::Diagonal>(1, 3, initial, nonjumping ? false : true, hopping, grasshopping, moveType, currentDirs));
		}
		};

	auto addPieceWithDist = [&](char piece, int dist) {
		if (piece == 'B' || piece == 'F' || piece == 'Q') {
			moveSets.emplace_back(std::make_shared<Chess::Diagonal>(dist, 1, initial, false, hopping, grasshopping, moveType, currentDirs));
		}
		if (piece == 'R' || piece == 'W' || piece == 'Q') {
			moveSets.emplace_back(std::make_shared<Chess::Orthogonal>(dist, 1, initial, false, hopping, grasshopping, moveType, currentDirs));
		}
		if (piece == 'N') {
			moveSets.emplace_back(std::make_shared<Chess::Leaper>(dist, 1, 2, initial, moveType, currentDirs));
		}
		if (piece == 'C') {
			moveSets.emplace_back(std::make_shared<Chess::Leaper>(dist, 1, 3, initial, moveType, currentDirs));
		}
		if (piece == 'Z') {
			moveSets.emplace_back(std::make_shared<Chess::Leaper>(dist, 2, 3, initial, moveType, currentDirs));
		}
		if (piece == 'K') {
			moveSets.emplace_back(std::make_shared<Chess::Adjacent>(initial, moveType, currentDirs));
		}
		if (piece == 'D') {
			moveSets.emplace_back(std::make_shared<Chess::Orthogonal>(dist, 2, initial, nonjumping ? false : true, hopping, grasshopping, moveType, currentDirs));
		}
		if (piece == 'A') {
			moveSets.emplace_back(std::make_shared<Chess::Diagonal>(dist, 2, initial, nonjumping ? false : true, hopping, grasshopping, moveType, currentDirs));
		}
		if (piece == 'H') {
			moveSets.emplace_back(std::make_shared<Chess::Orthogonal>(dist, 3, initial, nonjumping ? false : true, hopping, grasshopping, moveType, currentDirs));
		}
		if (piece == 'G') {
			moveSets.emplace_back(std::make_shared<Chess::Diagonal>(dist, 3, initial, nonjumping ? false : true, hopping, grasshopping, moveType, currentDirs));
		}
		};

	for (auto& str : splitMoveStr(moveStr, allPieces)) {
		if (str.find('m') != str.npos && str.find('c') != str.npos) {
			moveType = Chess::MoveType::All;
		}
		else if (str.find('c') != str.npos) {
			moveType = Chess::MoveType::Capture;
		}
		else if (str.find('m') != str.npos) {
			moveType = Chess::MoveType::Move;
		}
		for (int i = 0; i < str.size(); i++) {
			char c = str.at(i);
			// Directions
			if (c == 'f') {
				currentDirs.emplace(Chess::Direction::Up);
			}
			else if (c == 'b') {
				currentDirs.emplace(Chess::Direction::Down);
			}
			else if (c == 'r') {
				currentDirs.emplace(Chess::Direction::Right);
			}
			else if (c == 'l') {
				currentDirs.emplace(Chess::Direction::Left);
			}
			else if (c == 'v') {
				currentDirs.emplace(Chess::Direction::Up);
				currentDirs.emplace(Chess::Direction::Down);
			}
			else if (c == 's') {
				currentDirs.emplace(Chess::Direction::Left);
				currentDirs.emplace(Chess::Direction::Right);
			}
			else if (c == 'g') {
				grasshopping = true;
			}
			else if (c == 'p') {
				hopping = true;
			}
			else if (c == 'i') { 
				initial = true;
			}
			else if (c == 'n') {
				nonjumping = true;
			}
			// Move Sets
			else if (allPieces.find(c) != allPieces.npos) {
				if (i != str.size() - 1 && std::isdigit(str.at(i + 1))) {
					int dist = std::stoi(str.substr(i + 1));
					addPieceWithDist(c, dist);
				}
				else if (i != str.size() - 1 && c == str.at(i + 1)) {
					addPieceWithDist(c, 0);
				}
				else if ((i == 0) || (i != 0 && c != str.at(i - 1)))
				{
					addPiece(c);
				}
			}
			previous = c;
		}
		currentDirs.clear();
		moveType = Chess::MoveType::All;
		grasshopping = false;
		hopping = false;
		initial = false;
		nonjumping = false;
	}
	return moveSets;
}

pieceVector Chess::makePieceVec(const pieceVector& pieceList, sf::Vector2i square, sf::Vector2i localPos)
{
	if (square == localPos) { return pieceList; }

	pieceVector newV;
	newV.resize(pieceList.size());
	for (size_t j = 0; j < pieceList.size(); j++)
	{
		newV[j] = pieceList[j]->clone(false, false, false);
	}
	for (int j = 0; j < newV.size(); j++) {
		if (newV.at(j)->getLocalPos() == square) {
			newV.erase(newV.begin() + j);
			break;
		}
	}
	for (auto& v : newV) {
		if (v->getLocalPos() == localPos) {
			v->setLocalPosition(square);
			break;
		}
	}
	return newV;
}

pieceVector Chess::makePieceVec(const pieceVector& pieceList, sf::Vector2i square, const Piece& piece, const Chess::Variant& variant)
{
	if (square == piece.getLocalPos()) { return pieceList; }

	pieceVector newV;
	bool isCapture = false;
	for (const auto& piece : pieceList)
	{
		newV.push_back(piece->clone(false, false, false));
	}
	for (int j = 0; j < newV.size(); j++) {
		if (newV.at(j)->getLocalPos() == square) {
			newV.erase(newV.begin() + j);
			isCapture = true;
			break;
		}
	}
	for (auto& v : newV) {
		if (v->getLocalPos() == piece.getLocalPos()) {
			v->setLocalPosition(square);
			v->hasMoved = true;
			break;
		}
	}
	if (isCapture) {
		if (variant.atomicExplosions) {
			std::vector<sf::Vector2i> piecePositions;
			if (!variant.atomicImmunePieces.has(piece.id)) {
				piecePositions.push_back(square);
			}
			for (int x = -1; x <= 1; x++) {
				for (int y = -1; y <= 1; y++) {
					sf::Vector2i newPos = { square.x + x, square.y + y };
					if (Chess::isValidSquare(newPos, variant.boardSize)) {
						std::shared_ptr<Piece> p = Chess::getPieceFromPosition(newPos, newV);
						if (p && !variant.atomicImmunePieces.has(p->id) && !variant.pawnPiecesBlastImmunity.has(p->id)) {
							piecePositions.push_back(newPos);
						}
					}
				}
			}
			for (auto& pos : piecePositions) {
				for (auto it2 = newV.begin(); it2 != newV.end(); it2++) {
					std::shared_ptr<Piece> p = *it2;
					if (p->getLocalPos() == pos) {
						newV.erase(it2);
						break;
					}
				}
			}
		}
	}
	return newV;
}

pieceVector Chess::copyPieceVec(const pieceVector& pieceList, bool copySprites, bool copySquares, bool copyPromotionTypes)
{
	pieceVector newV;
	for (auto& piece : pieceList) {
		newV.push_back(piece->clone(copySprites, copySquares, copyPromotionTypes));
	}
	return newV;
}

Chess::WinValue Chess::getWinValue(const std::string& valueStr) {
	if (valueStr == "win") {
		return Chess::WinValue::Win;
	}
	else if (valueStr == "loss") {
		return Chess::WinValue::Loss;
	}
	else if (valueStr == "draw") {
		return Chess::WinValue::Draw;
	}
	return Chess::WinValue::None;
}

bool Chess::isValidSquare(sf::Vector2i square, sf::Vector2u boardSize) {
	return (square.x <= int(boardSize.x) && square.x >= 1 && square.y <= int(boardSize.y) && square.y >= 1);
}

bool Chess::isValidPosition(const Chess::Position& position, const Chess::Variant& variant, Chess::PColor color) {
	for (auto& piece : position.pieceList) {
		if (piece->color != color && !piece->validatePosition(position, variant)) {
			return false;
		}
	}
	return true;
}

bool Chess::isValidFlyingGeneral(const Chess::Position& position, const Chess::Variant& variant, Chess::PColor color) {
	for (auto& piece : position.pieceList) {
		if (piece && variant.royalPieces.has(piece->id) && piece->color == color) {
			for (int i = piece->getLocalPos().y + 1; i <= variant.boardSize.y; i++) {
				sf::Vector2i newPos = { piece->getLocalPos().x, i };
				auto p = Chess::getPieceFromPosition(newPos, position.pieceList);
				if (p) {
					if (variant.royalPieces.has(p->id) && p->color != color) {
						return false;
					}
					else { break; }
				}
			}
			for (int i = piece->getLocalPos().y - 1; i > 0; i--) {
				sf::Vector2i newPos = { piece->getLocalPos().x, i };
				auto p = Chess::getPieceFromPosition(newPos, position.pieceList);
				if (p) {
					if (variant.royalPieces.has(p->id) && p->color != color) {
						return false;
					}
					else { break; }
				}
			}
			for (int i = piece->getLocalPos().x + 1; i <= variant.boardSize.x; i++) {
				sf::Vector2i newPos = { i, piece->getLocalPos().y };
				auto p = Chess::getPieceFromPosition(newPos, position.pieceList);
				if (p) {
					if (variant.royalPieces.has(p->id) && p->color != color) {
						return false;
					}
					else { break; }
				}
			}
			for (int i = piece->getLocalPos().x - 1; i > 0; i--) {
				sf::Vector2i newPos = { i, piece->getLocalPos().y };
				auto p = Chess::getPieceFromPosition(newPos, position.pieceList);
				if (p) {
					if (variant.royalPieces.has(p->id) && p->color != color) {
						return false;
					}
					else { break; }
				}
			}
		}
	}
	return true;
}

bool Chess::isValidAtomicCapture(sf::Vector2i square, const Chess::Position& position, const Chess::Variant& variant, Chess::PColor color) {
	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			sf::Vector2i newPos = { square.x + x, square.y + y };
			if (Chess::isValidSquare(newPos, variant.boardSize) && Chess::getPieceFromPosition(newPos, position.pieceList)) {
				std::shared_ptr<Piece> p = Chess::getPieceFromPosition(newPos, position.pieceList);
				if (p->color == color && !variant.atomicImmunePieces.has(p->id) && !variant.pawnPiecesBlastImmunity.has(p->id) && Chess::isRoyal(p, variant, position.extinctionRoyalPieces)) {
					return false;
				}		
			}
		}
	}
	return true;
}

bool Chess::canCaptureEnemyKing(sf::Vector2i square, const Chess::Position& position, const Chess::Variant& variant, Chess::PColor color) {
	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			sf::Vector2i newPos = { square.x + x, square.y + y };
			if (Chess::isValidSquare(newPos, variant.boardSize)) {
				std::shared_ptr<Piece> p = Chess::getPieceFromPosition(newPos, position.pieceList);
				if (x == 0 && y == 0) {
					if (p && p->color != color && Chess::isRoyal(p, variant, position.extinctionRoyalPieces)) {
						return true;
					}
				}
				else {
					if (p && p->color != color && !variant.atomicImmunePieces.has(p->id) && !variant.pawnPiecesBlastImmunity.has(p->id) && Chess::isRoyal(p, variant, position.extinctionRoyalPieces)) {
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool Chess::enemyKingOccupies(sf::Vector2i square, const Chess::Position& position, const Chess::Variant& variant, Chess::PColor color) {
	for (auto& piece : position.pieceList) {
		if (variant.hasPiece(piece->id) && piece->color != color && Chess::isRoyal(piece, variant, position.extinctionRoyalPieces)) {
			for (const auto& set : variant.pieceMoves.find(piece->id)->second) {
				bool moved = variant.sRanks.at(piece->getLocalPos().y - 1).at(piece->getLocalPos().x - 1) != (piece->isWhite() ? std::toupper(piece->id) : piece->id);
				if (!moved || !set->initial) {
					auto pieceMoves = set->getSquares(position.pieceList, piece->color, piece->getLocalPos(), variant.boardSize);
					for (const auto& move : pieceMoves.moveSet) {
						if (move == square) { return true; }
					}
					for (const auto& capture : pieceMoves.moveSet) {
						if (capture == square) { return true; }
					}
				}
			}
		}
	}
	return false;
}

bool Chess::isInCenter(sf::Vector2i pos)
{
	return (pos.x == 4 || pos.x == 5) && (pos.y == 4 || pos.y == 5);
}

sf::Vector2f Interpolate(const sf::Vector2f pointA, const sf::Vector2f pointB, float factor) {
	if (std::fmax(std::abs((pointB - pointA).x), std::abs((pointB - pointA).y)) < 1.0f) {
		return pointB;
	}
	return (pointA * (1 - factor) + pointB * factor);
}
sf::Vector2f Interpolate(const sf::Vector2f pointA, const sf::Vector2f pointB, float factor, float threshold) {
	factor = std::clamp(factor, 0.0f, 1.0f);
	if (std::fmax(std::abs((pointB - pointA).x), std::abs((pointB - pointA).y)) < threshold) {
		return pointB;
	}
	return (pointA * (1 - factor) + pointB * factor);
}

std::string Chess::getNewFEN(const Chess::Variant& variant, bool chess960Enabled)
{
	std::string nFEN = variant.startingFEN;
	if (chess960Enabled && variant.boardSize == sf::Vector2u{8, 8}) {
		int id = 0;
		std::string fen = Chess::getRandomLineFrom("assets/fen/chess960.txt", id);
		if (!fen.empty()) {
			if (variant.name == "horde") {
				nFEN = fen.substr(0, 8) + "/pppppppp/8/1PP2PP1/PPPPPPPP/PPPPPPPP/PPPPPPPP/PPPPPPPP";
			}
			else if (variant.name == "racingkings") {
				nFEN = "8/8/8/8/8/8/krbnNBRK/qrbnNBRQ";
			}
			else {
				nFEN = fen;
			}
			if ((variant.name == "seirawan" || variant.name == "shouse") && chess960Enabled) {
				nFEN += "[HEhe] w KQABCDEFGHkqabcdefgh - 0 1";
			}
			if (variant.startingFEN.find('[') != variant.startingFEN.npos) {
				nFEN += variant.startingFEN.substr(variant.startingFEN.find_first_of('['));
			}
			else {
				if (variant.startingFEN.find(' ') != variant.startingFEN.npos) {
					nFEN += variant.startingFEN.substr(variant.startingFEN.find_first_of(' '));
				}
			}
		}
		else {
			std::cout << "Empty Chess960 string returnd! ID: " << id << std::endl;
		}
	}
	return nFEN;
}

std::pair<Chess::PieceType, Chess::PieceType> Chess::getExtinctionRoyalPieces(const Chess::Variant& variant, const pieceVector& pieceList)
{
	Chess::PieceType royalWhite, royalBlack;
	if (variant.extinctionPseudoRoyal) {
		for (auto& p : variant.whiteExtinctionPieces) {
			if (std::count_if(pieceList.cbegin(), pieceList.cend(), [&variant, p](const std::shared_ptr<Piece>& piece) {
				return piece->isWhite() && piece->id == p; }) <= (variant.extinctionPieceCount + 1)) {
				royalWhite.add(p);
			}
		}
		for (auto& p : variant.blackExtinctionPieces) {
			if (std::count_if(pieceList.cbegin(), pieceList.cend(), [&variant, p](const std::shared_ptr<Piece>& piece) {
				return piece->isBlack() && piece->id == p; }) <= (variant.extinctionPieceCount + 1)) {
				royalBlack.add(p);
			}
		}
	}
	return { royalWhite, royalBlack };
}

bool Chess::isExtinctionRoyal(const std::shared_ptr<Piece>& piece, const std::pair<Chess::PieceType, Chess::PieceType>& extinctionRoyalPieces) {
	return (piece->isWhite() && extinctionRoyalPieces.first.has(piece->id))
		|| (piece->isBlack() && extinctionRoyalPieces.second.has(piece->id));
}

bool Chess::isExtinctionRoyal(const Piece* piece, const std::pair<Chess::PieceType, Chess::PieceType>& extinctionRoyalPieces) {
	return (piece->isWhite() && extinctionRoyalPieces.first.has(piece->id))
		|| (piece->isBlack() && extinctionRoyalPieces.second.has(piece->id));
}

bool Chess::isRoyal(const std::shared_ptr<Piece>& piece, const Chess::Variant& variant, const std::pair<Chess::PieceType, Chess::PieceType>& extinctionRoyalPieces)
{
	return variant.royalPieces.has(piece->id) || (variant.extinctionPseudoRoyal && Chess::isExtinctionRoyal(piece, extinctionRoyalPieces));
}

bool Chess::isRoyal(const Piece* piece, const Chess::Variant& variant, const std::pair<Chess::PieceType, Chess::PieceType>& extinctionRoyalPieces)
{
	return variant.royalPieces.has(piece->id) || (variant.extinctionPseudoRoyal && Chess::isExtinctionRoyal(piece, extinctionRoyalPieces));
}

uint8_t Interpolate(uint8_t current, uint8_t target, float factor)
{
	return (uint8_t)current + (target - current) * factor;
}

void setSpriteVisible(sf::Sprite& sprite, bool visible)
{
	if (visible) {
		sprite.setColor({ sprite.getColor().r, sprite.getColor().g, sprite.getColor().b, 255 });
	}
	else {
		sprite.setColor({ sprite.getColor().r, sprite.getColor().g, sprite.getColor().b, 0 });
	}
}

void setSpriteVisible(sf::Sprite& sprite, bool visible, uint8_t vAlpha)
{
	if (visible) {
		sprite.setColor({ sprite.getColor().r, sprite.getColor().g, sprite.getColor().b, vAlpha });
	}
	else {
		sprite.setColor({ sprite.getColor().r, sprite.getColor().g, sprite.getColor().b, 0 });
	}
}

std::vector<std::string> split(const std::string& str, char del) {
	std::vector<std::string> res;
	std::string temp;
	for (int i = 0; i < str.size(); i++) {
		if (str.at(i) == del) {
			res.push_back(temp);
			temp = "";
		}
		else {
			temp += str.at(i);
		}
	}
	res.push_back(temp);
	return res;
}

std::vector<std::string> split_if_not(const std::string& str, char del, char ignore) {
	std::vector<std::string> res;
	std::string temp;
	for (int i = 0; i < str.size(); i++) {
		if (str.at(i) == ignore) { continue; }
		if (str.at(i) == del) {
			if (!temp.empty()) {
				res.push_back(temp);
				temp = "";
			}
		}
		else {
			temp += str.at(i);
		}
	}
	if (!temp.empty()) {
		res.push_back(temp);
	}
	return res;
}

std::vector<std::string> splitMoveStr(const std::string& str, const std::string& pieces)
{
	std::vector<std::string> res;
	if (!str.empty()) {
		auto piecesHas = [&](char c) {
			for (auto& piece : pieces) {
				if (piece == c) { return true; }
			}
			return false;
			};

		std::string temp;
		for (int i = 0; i < str.size() - 1; i++) {
			temp += str.at(i);
			if ((piecesHas(str.at(i)) && str.at(i) != str.at(i + 1) && !std::isdigit(str.at(i + 1)))
				|| (std::isdigit(str.at(i)) && !std::isdigit(str.at(i + 1)))) {
				res.push_back(temp);
				temp = "";
			}
		}
		temp += str.back();
		res.push_back(temp);
	}
	return res;
}

bool strToBool(const std::string& str)
{
	bool newBool = false;
	if (str == "true") {
		newBool = true;
	}
	else if (str != "false") {
		std::cerr << "String to bool conversion failed! String: " << str << "." << std::endl;
	}
	return newBool;
}

std::string Chess::getRandomLineFrom(std::string filePath)
{
	std::ifstream readFile(filePath);
	std::string line;
	int lines = 0;
	while (std::getline(readFile, line)) { lines++; }
	int id = (std::rand() % lines) + 1;
	readFile.clear();
	readFile.seekg(0, readFile.beg);
	line.clear();
	for (int i = 1; i <= id; i++) {
		std::getline(readFile, line);
	}
	readFile.close();
	return line;
}

std::string Chess::getRandomLineFrom(std::string filePath, int& id)
{
	std::ifstream readFile(filePath);
	std::string line;
	int lines = 0;
	while (std::getline(readFile, line)) { lines++; }
	id = (std::rand() % lines) + 1;
	readFile.clear();
	readFile.seekg(0, readFile.beg);
	line.clear();
	for (int i = 1; i <= id; i++) {
		std::getline(readFile, line);
	}
	readFile.close();
	return line;
}

bool randomBool() {
	static auto gen = std::bind(std::uniform_int_distribution<>(0, 1), std::default_random_engine());
	return gen();
}

Chess::Position::Position() : atomicKings(false), hasChess960(false), enPassantTarget({}), gameEnded(false),
	castlingRights{true, true, true, true}, castlePieces{-1, -1, -1, -1}, extinctionRoyalPieces("", ""), extraFlagMove(false)
{
}

// Misc
size_t findNthOf(const std::string& str, char find, int nth)
{
	int occ = 0;
	for (size_t i = 0; i < str.size(); i++) {
		if (str.at(i) == find) { occ++; }
		if (occ == nth) { return i; }
	}
	return 0;
}
