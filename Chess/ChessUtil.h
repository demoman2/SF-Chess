#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Chess.h"

class Piece;

using pieceVector = std::vector<std::shared_ptr<Piece>>;
namespace Chess {

	struct Square {
		sf::Vector2i pos;
		std::optional<sf::Sprite> sprite;
		std::string moveString;

		Square(sf::Sprite& sprite, sf::Vector2i position);
		Square(sf::Vector2i position, std::string moveString);
		~Square() {};
		void setupSprite(sf::Texture& texture, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, float pieceScale, bool reversed);
		void setTexture(sf::Texture& texture);
	};

	struct DropPiece {
		char id;
		int count;
		sf::Sprite sprite, background, textBG;
		sf::Text text;

		DropPiece(char id, sf::Texture& texture, sf::Texture& backgroundTexture, sf::Texture& textBackgroundTexture, sf::Font& font, sf::Vector2f dropPiecePosition, float dropPieceSquareSize, float bgStart, size_t i);
		~DropPiece() {};
		void update();
		void move(float x, float y);
		void scale(float dropPieceSquareSize, sf::Sprite& dropPieceBackground, float bgStart, int i);
		bool mouseSelecting(sf::Vector2f mousePos) const;
		void draw(sf::RenderWindow& window) const;
	};
	struct SDropPiece {
		char id;
		sf::Sprite sprite;

		SDropPiece(char id, sf::Texture& texture);
		~SDropPiece() {};
		void set(sf::Texture& texture, int id);
	};

	struct PromotePiece {
		sf::Sprite sprite, backgroundSprite;
		sf::Vector2f initialScale;
		char id;

		PromotePiece(sf::Texture& pieceTexture, sf::Texture& backgroundTexture, sf::Vector2i pos, char id, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, float pieceScale, sf::Vector2f boardScale, sf::Color promotionSquareColor, sf::Vector2f scale);
		~PromotePiece() {};
	};

	std::vector<sf::Texture> makePieceSet(sf::Image spriteSheet, int pieceCount, int pieceSize);
	void loadPieceSet(sf::Image& spriteSheet, std::vector<sf::Texture>& pieceTextures, int pieceSize);
	void loadBoard(sf::Image& spriteSheet, sf::Sprite& board, sf::Texture& boardTexture, int boardNumber, int boardSize);

	bool isPieceAt(sf::Vector2i position, const std::vector<std::shared_ptr<Piece>>& pieces);
	std::shared_ptr<Piece> getPieceFromPosition(sf::Vector2i position, const std::vector<std::shared_ptr<Piece>>& pieces);
	std::vector<std::shared_ptr<Piece>> makePieceVec(const std::vector<std::shared_ptr<Piece>>& pieceList, sf::Vector2i square, sf::Vector2i localPos);
	std::vector<std::shared_ptr<Piece>> makePieceVec(const std::vector<std::shared_ptr<Piece>>& pieceList, sf::Vector2i square, sf::Vector2i localPos, const Chess::Variant variant);

	bool isValidSquare(sf::Vector2i square);
	bool isValidPosition(const std::vector<std::shared_ptr<Piece>>& position, Chess::PColor color);
	bool isValidAtomicCapture(sf::Vector2i square, const std::vector<std::shared_ptr<Piece>>& pieces, Chess::PColor color);
	bool canCaptureEnemyKing(sf::Vector2i square, const std::vector<std::shared_ptr<Piece>>& pieces, Chess::PColor color);
	bool enemyKingOccupies(sf::Vector2i square, const std::vector<std::shared_ptr<Piece>>& pieces, Chess::PColor color);
	bool isInCenter(sf::Vector2i pos);

	bool isValidDropSquare(char id, sf::Vector2i pos);
}

std::vector<std::string> split(std::string str, char del);
size_t findNthOf(std::string str, char find, int nth);


sf::Vector2f Interpolate(const sf::Vector2f pointA, const sf::Vector2f pointB, float factor);
sf::Vector2f Interpolate(const sf::Vector2f pointA, const sf::Vector2f pointB, float factor, float threshold);
uint8_t Interpolate(uint8_t current, uint8_t target, float factor);

void setSpriteVisible(sf::Sprite& sprite, bool visible);
void setSpriteVisible(sf::Sprite& sprite, bool visible, uint8_t vAlpha);