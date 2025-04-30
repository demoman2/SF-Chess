#pragma once
#include "Piece.h"

class Knight : public virtual Piece
{

public:
	Knight(int x, int y, float scale, float boardXOffset, float boardMultiplier, PColor color, sf::Texture& texture, bool animated);
	~Knight();
	virtual std::shared_ptr<Piece> clone() const override;
};

