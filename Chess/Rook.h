#pragma once
#include "Piece.h"

class Rook : public Piece
{
	bool hasMoved, kingSide, queenSide;
public:
	Rook(int x, int y, float scale, float xOffset, float boardMultiplier, PColor color, sf::Texture& texture, std::vector<std::unique_ptr<Piece>>& pieces);
	Rook(int x, int y, bool kingSide, bool hasMoved, float scale, float xOffset, float boardMultiplier, PColor color, sf::Texture& texture, std::vector<std::unique_ptr<Piece>>& pieces);
	void calculatePositions();
};

