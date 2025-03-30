#pragma once
#include "Piece.h"

class Bishop : public Piece
{
	
public:
	Bishop(int x, int y, float scale, float xOffset, float boardMultiplier, PColor color, sf::Texture& texture);
	void calculatePositions();
};

