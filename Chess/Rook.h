#pragma once
#include "Piece.h"

class Rook : public virtual Piece
{

public:
	bool hasMoved;
	Rook(int x, int y, float scale, float boardOffset, float boardMultiplier, size_t index, PColor color, sf::Texture& texture);
	~Rook();
	virtual std::shared_ptr<Piece> clone() const override;
};

