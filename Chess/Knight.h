#pragma once
#include "Piece.h"

class Knight : public virtual Piece
{

public:
	Knight(int x, int y, float scale, float boardOffset, float boardMultiplier, int index, PColor color, sf::Texture& texture);
	~Knight();
};

