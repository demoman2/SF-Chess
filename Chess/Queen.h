#pragma once
#include "Piece.h"

class Queen : public virtual Piece
{

public:
	Queen(int x, int y, float scale, float boardOffset, float boardMultiplier, int index, PColor color, sf::Texture& texture);
	~Queen();
};

