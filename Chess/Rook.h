#pragma once
#include "Piece.h"

class Rook : public virtual Piece
{
public:
	bool hasMoved;
	Rook(int x, int y, float scale, float xOffset, float boardMultiplier, int index, PColor color, sf::Texture& texture);
	~Rook();
	void a();
};

