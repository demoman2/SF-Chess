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
#include <vector>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <random>
#include <functional>
#include <set>
#include "PieceType.h"

class Piece;

using pieceVector = std::vector<std::shared_ptr<Piece>>;

namespace Chess
{
	class Variant;

	class MoveSet;

	enum class PColor {
		White,
		Black
	};
	enum class Endgame {
		None,
		Checkmate,
		InsufficientMaterial,
		FiftyMoveRule,
		ThreefoldRepetition,
		VariantVictory,
		Draw,
		VariantLoss
	};
	enum class GameType {
		UltraBullet,
		Bullet,
		Blitz,
		Rapid,
		Classical
	};
	enum class WinValue {
		Win,
		Loss,
		Draw,
		None
	};
	static PColor operator!(PColor c) {
		return c == Chess::PColor::White ? Chess::PColor::Black : Chess::PColor::White;
	}

	struct vector2i_less {
		constexpr bool operator()(const sf::Vector2i& a, const sf::Vector2i& b) const {
			return (a.x < b.x) || (a.x == b.x && a.y < b.y);
		}
	};

	using SquareSet = std::set<sf::Vector2i, Chess::vector2i_less>;

	struct Position {

	public:
		pieceVector pieceList;
		bool atomicKings, hasChess960;

		// En Passant
		std::optional<sf::Vector2i> enPassantTarget, enPassantTripleTarget;

		// Castling
		std::array<int, 4> castlePieces; // wK, wQ, bK, bQ
		std::array<bool, 4> castlingRights; // wK, wQ, bK, bQ

		bool extraFlagMove, gameEnded;

		// Pseudo Royal
		std::pair<Chess::PieceType, Chess::PieceType> extinctionRoyalPieces;

		Position();
		~Position() {};
	};

	struct Square {

	public:
		bool promoteSquare;
		Chess::PieceType validPromotionTypes;
		sf::Vector2i pos;
		std::string moveString;
		std::unique_ptr<sf::Sprite> sprite;

		Square(sf::Sprite& sprite, sf::Vector2i position, const Chess::PieceType& validPromotionTypes = {}, bool promoteSquare = false);
		Square(sf::Vector2i position, std::string moveString, const Chess::PieceType& validPromotionTypes = {}, bool promoteSquare = false);
		Square(const Square& other);
		
		Square& operator=(const Square& other);
		
		~Square() {};
		
		void setupSprite(const sf::Texture& texture, sf::Vector2f boardOffset, sf::Vector2f boardSize, sf::Vector2u boardSquares, float boardMultiplier, bool reversed);
		void setTexture(const sf::Texture& texture);

	};

	sf::Vector2i convertChessNotationtoPosition(const std::string& notation);
	std::string convertPositiontoNotation(sf::Vector2i position);

	int convertXtoChar(int x);
	int convertChartoX(char c);

	template<typename T> static T reverseY(T y, sf::Vector2u boardSize) { return boardSize.y - y + 1; }
	sf::Vector2i reversePositon(sf::Vector2i position, sf::Vector2u boardSquares);
	sf::Vector2f reversePosition(sf::Vector2f position, sf::Vector2f boardOffset);

	sf::Vector2f getGlobalPosition(sf::Vector2i position, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, sf::Vector2u boardSquares, bool reversed = false);
	sf::Vector2f getGlobalPosition(sf::Vector2i position, sf::Vector2f boardOffset, sf::Vector2f boardSize, sf::Vector2f boardMultiplier, sf::Vector2u boardSquares, bool reversed);
	sf::Vector2f getGlobalPosition(sf::Vector2f position, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, sf::Vector2u boardSquares, bool reversed = false);
	sf::Vector2i getLocalPosition(sf::Vector2f position, sf::Vector2f boardOffset, float boardMultiplier, sf::Vector2u boardSquares, bool reversed = false);

	bool isValidSquare(sf::Vector2i square, sf::Vector2u boardSize);
	bool isValidPosition(const Chess::Position& position, const Chess::Variant& variant, Chess::PColor color);
	bool isValidFlyingGeneral(const Chess::Position& position, const Chess::Variant& variant, Chess::PColor color);
	bool isValidAtomicCapture(sf::Vector2i square, const Chess::Position& position, const Chess::Variant& variant, Chess::PColor color);
	bool canCaptureEnemyKing(sf::Vector2i square, const Chess::Position& position, const Chess::Variant& variant, Chess::PColor color);
	bool enemyKingOccupies(sf::Vector2i square, const Chess::Position& position, const Chess::Variant& variant, Chess::PColor color);
	bool isInCenter(sf::Vector2i pos);

	pieceVector makePieceVec(const pieceVector& pieceList, sf::Vector2i square, sf::Vector2i localPos);
	pieceVector makePieceVec(const pieceVector& pieceList, sf::Vector2i square, const Piece& piece, const Chess::Variant& variant);
	pieceVector copyPieceVec(const pieceVector& pieceList, bool copySprites = false, bool copySquares = true, bool copyPromotionTypes = true);

	Chess::WinValue getWinValue(const std::string& valueStr);

	bool isPieceAt(sf::Vector2i position, const pieceVector& pieces);
	std::shared_ptr<Piece> getPieceFromPosition(sf::Vector2i position, const pieceVector& pieces);

	std::vector<std::shared_ptr<Chess::MoveSet>> getMoveSets(const std::string& moveStr);

	std::string getRandomLineFrom(std::string filePath);
	std::string getRandomLineFrom(std::string filePath, int& id);
	std::string getNewFEN(const Chess::Variant& variant, bool chess960Enabled);

	std::pair<Chess::PieceType, Chess::PieceType> getExtinctionRoyalPieces(const Chess::Variant& variant, const pieceVector& pieceList);
	bool isExtinctionRoyal(const std::shared_ptr<Piece>& piece, const std::pair<Chess::PieceType, Chess::PieceType>& extinctionRoyalPieces);
	bool isExtinctionRoyal(const Piece* piece, const std::pair<Chess::PieceType, Chess::PieceType>& extinctionRoyalPieces);
	bool isRoyal(const std::shared_ptr<Piece>& piece, const Chess::Variant& variant, const std::pair<Chess::PieceType, Chess::PieceType>& extinctionRoyalPieces);
	bool isRoyal(const Piece* piece, const Chess::Variant& variant, const std::pair<Chess::PieceType, Chess::PieceType>& extinctionRoyalPieces);
}

sf::Vector2f Interpolate(const sf::Vector2f pointA, const sf::Vector2f pointB, float factor);
sf::Vector2f Interpolate(const sf::Vector2f pointA, const sf::Vector2f pointB, float factor, float threshold);
uint8_t Interpolate(uint8_t current, uint8_t target, float factor);

void setSpriteVisible(sf::Sprite& sprite, bool visible);
void setSpriteVisible(sf::Sprite& sprite, bool visible, uint8_t vAlpha);

std::vector<std::string> split(const std::string& str, char del);
std::vector<std::string> split_if_not(const std::string& str, char del, char ignore);
std::vector<std::string> splitMoveStr(const std::string& str, const std::string& pieces);
bool strToBool(const std::string& str);

template<typename Container, typename T>
bool contains(const Container& container, const T& element);

bool randomBool();

template<typename Container, typename T>
inline bool contains(const Container& container, const T& element)
{
	return std::find(container.begin(), container.end(), element) != container.end();
}

size_t findNthOf(const std::string& str, char find, int nth);