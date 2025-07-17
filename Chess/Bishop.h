#pragma once
#include "Piece.h"

class Bishop : public virtual Piece
{
	
public:
	Bishop(int x, int y, float scale, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, Chess::PColor color, sf::Texture& texture, bool animated, bool promoted, bool reversed);
	Bishop(const Bishop& other, bool copySprite = false, bool copySquares = true);
	~Bishop();

	// Virtual Functions
	std::shared_ptr<Piece> clone(bool copySprite = false, bool copySquares = true) const override;
	void generateLegalMoves(const pieceVector& pieceList, Chess::Variant variant, bool atomicKings, bool checksEnabled, bool castlingEnabled, bool chess960) override;
	bool validatePosition(const pieceVector& pieceList) override;
};