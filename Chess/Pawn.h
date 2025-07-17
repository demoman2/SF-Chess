#pragma once
#include "Piece.h"

class Pawn : public virtual Piece
{

public:
	bool enPassantTarget;

	Pawn(int x, int y, float scale, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, Chess::PColor color, sf::Texture& texture, bool animated, bool promoted, bool reversed);
	Pawn(bool enPassantTarget, int x, int y, float scale, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, Chess::PColor color, sf::Texture& texture, bool animated, bool promoted, bool reversed);
	Pawn(const Pawn& other, bool copySprite = false, bool copySquares = true);
	~Pawn();

	// Virtual Functions
	std::shared_ptr<Piece> clone(bool copySprite = false, bool copySquares = true) const override;
	void generateLegalMoves(const pieceVector& pieceList, Chess::Variant variant, bool atomicKings, bool checksEnabled, bool castlingEnabled, bool chess960) override;
	bool validatePosition(const pieceVector& pieceList) override;
	void updateSprites(std::vector<sf::Texture>& boardTextures, sf::Vector2f mousePos, bool& mouseSelecting) override;
	void afterMovePlayed() override { enPassantTarget = false; }

	std::vector<Chess::Square>& getEnPassantSquares() { return enPassantPositions; }

private:
	std::vector<Chess::Square> enPassantPositions;
	void addEnPassantSquare(sf::Vector2i square);
	bool isValidPassant(sf::Vector2i square, const pieceVector& pieceList, Chess::Variant variant, bool atomicKings, bool checksEnabled);
};

