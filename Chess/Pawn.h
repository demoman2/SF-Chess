#pragma once
#include "Piece.h"

class Pawn : public Piece
{

public:
	bool enPassantTarget;
	Pawn(int x, int y, float scale, float xOffset, float boardMultiplier, PColor color, sf::Texture& texture);
	Pawn(bool enPassantTarget, int x, int y, float scale, float xOffset, float boardMultiplier, PColor color, sf::Texture& texture);
	void calculatePositions();
};

