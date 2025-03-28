#pragma once
#include "Piece.h"

class Knight : public Piece
{

public:
	Knight(int x, int y, float scale, float xOffset, float boardMultiplier, PColor color, sf::Texture& texture, std::vector<std::unique_ptr<Piece>>& pieces);
	void calculatePositions();
};

