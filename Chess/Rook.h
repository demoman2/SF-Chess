#pragma once
#include "Piece.h"

class Rook : public Piece
{
public:
	bool hasMoved;
	Rook(int x, int y, float scale, float xOffset, float boardMultiplier, PColor color, sf::Texture& texture);
	void calculatePositions();
};

