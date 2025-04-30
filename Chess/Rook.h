#pragma once
#include "Piece.h"

class Rook : public virtual Piece
{

public:
	Rook(int x, int y, float scale, float boardXOffset, float boardMultiplier, PColor color, sf::Texture& texture, bool animated);
	~Rook();
	virtual std::shared_ptr<Piece> clone() const override;
};

