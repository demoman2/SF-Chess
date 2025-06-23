#pragma once
#include "Piece.h"

class Rook : public virtual Piece
{

public:
	Rook(int x, int y, float scale, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, Chess::PColor color, sf::Texture& texture, bool animated, bool promoted, bool reversed);
	~Rook();

	// Virtual Functions	
	std::shared_ptr<Piece> clone() const override;
	void generateLegalMoves(const pieceVector& pieceList, Chess::Variant variant, bool atomicKings, bool checksEnabled, bool castlingEnabled, bool chess960, bool hasDoubleCheck) override;
	bool validatePosition(const pieceVector& pieceList) override;
};

