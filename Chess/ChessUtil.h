#pragma once
#include <string>
#include <vector>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <fstream>

class Piece;

using pieceVector = std::vector<std::shared_ptr<Piece>>;

namespace Chess
{

	enum PColor {
		White,
		Black
	};
	enum Endgame {
		None,
		Stalemate,
		Checkmate,
		InsufficientMaterial,
		FiftyMoveRule,
		ThreefoldRepetition,
		VariantVictory,
		VariantLoss
	};
	enum Variant {
		Standard,
		Atomic,
		Antichess,
		Horde,
		ThreeCheck,
		FiveCheck,
		RacingKings,
		KOTH,
		Crazyhouse
	};
	enum GameType {
		UltraBullet,
		Bullet,
		Blitz,
		Rapid,
		Classical
	};
	static PColor operator!(PColor c) {
		return static_cast<PColor>(c + 1 % 2);
	}

	struct Square {
		std::unique_ptr<sf::Sprite> sprite;
		sf::Vector2i pos;
		std::string moveString;

		Square(sf::Sprite& sprite, sf::Vector2i position);
		Square(sf::Vector2i position, std::string moveString);
		Square(const Square& other);
		Square& operator=(const Square& other);
		~Square() {};
		void setupSprite(sf::Texture& texture, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, float pieceScale, bool reversed);
		void setTexture(sf::Texture& texture);
	};

	std::string toString(Variant variant);

	sf::Vector2i convertChessNotationtoPosition(std::string notation);
	std::string convertPositiontoNotation(sf::Vector2i position);

	int convertXtoChar(int x);
	int convertChartoX(char c);

	template<typename T> static T reverseY(T y) { return 9 - y; }
	sf::Vector2i reversePositon(sf::Vector2i position);
	sf::Vector2f reversePosition(sf::Vector2f position, sf::Vector2f boardOffset);

	sf::Vector2f getGlobalPosition(sf::Vector2i position, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, bool reversed = false);
	sf::Vector2f getGlobalPosition(sf::Vector2f position, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, bool reversed = false);
	sf::Vector2i getLocalPosition(sf::Vector2f position, sf::Vector2f boardOffset, float boardMultiplier, bool reversed = false);

	bool isValidSquare(sf::Vector2i square);
	bool isValidPosition(const pieceVector& position, Chess::PColor color);
	bool isValidAtomicCapture(sf::Vector2i square, const pieceVector& pieces, Chess::PColor color);
	bool canCaptureEnemyKing(sf::Vector2i square, const pieceVector& pieces, Chess::PColor color);
	bool enemyKingOccupies(sf::Vector2i square, const pieceVector& pieces, Chess::PColor color);
	bool isInCenter(sf::Vector2i pos);

	pieceVector makePieceVec(const pieceVector& pieceList, sf::Vector2i square, sf::Vector2i localPos);
	pieceVector makePieceVec(const pieceVector& pieceList, sf::Vector2i square, sf::Vector2i localPos, const Chess::Variant variant);
	pieceVector copyPieceVec(const pieceVector& pieceList, bool copySprites = false, bool copySquares = true);

	bool isPieceAt(sf::Vector2i position, const pieceVector& pieces);
	std::shared_ptr<Piece> getPieceFromPosition(sf::Vector2i position, const pieceVector& pieces);

	std::string getRandomLineFrom(std::string filePath);
	std::string getRandomLineFrom(std::string filePath, int& id);
}

sf::Vector2f Interpolate(const sf::Vector2f pointA, const sf::Vector2f pointB, float factor);
sf::Vector2f Interpolate(const sf::Vector2f pointA, const sf::Vector2f pointB, float factor, float threshold);
uint8_t Interpolate(uint8_t current, uint8_t target, float factor);

void setSpriteVisible(sf::Sprite& sprite, bool visible);
void setSpriteVisible(sf::Sprite& sprite, bool visible, uint8_t vAlpha);

