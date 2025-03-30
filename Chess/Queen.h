#pragma once
#include "Piece.h"

class Queen : public Piece
{

public:
	Queen(int x, int y, float scale, float xOffset, float boardMultiplier, PColor color, sf::Texture& texture);
	void calculatePositions();
};

