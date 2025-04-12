#pragma once
#include "Piece.h"

class Knight : public virtual Piece
{

public:
	Knight(int x, int y, float scale, float boardOffset, float boardMultiplier, size_t index, PColor color, sf::Texture& texture, bool animated);
	~Knight();
	virtual std::shared_ptr<Piece> clone() const override;
};

