#pragma once
#include "Piece.h"

class Queen : public virtual Piece
{

public:
	Queen(int x, int y, float scale, float boardXOffset, float boardMultiplier, PColor color, sf::Texture& texture, bool animated);
	~Queen();
	virtual std::shared_ptr<Piece> clone() const override;
};

