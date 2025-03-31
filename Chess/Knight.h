#pragma once
#include "Piece.h"

class Knight : public virtual Piece
{

public:
	Knight(int x, int y, float scale, float xOffset, float boardMultiplier, PColor color, sf::Texture& texture);
	void calculatePositions() override;
};

