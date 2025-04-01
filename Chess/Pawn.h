#pragma once
#include "Piece.h"

class Pawn : public virtual Piece
{

public:
	bool enPassantTarget;
	Pawn(int x, int y, float scale, float xOffset, float boardMultiplier, int index, PColor color, sf::Texture& texture);
	Pawn(bool enPassantTarget, int x, int y, float scale, float xOffset, float boardMultiplier, int index, PColor color, sf::Texture& texture);
	~Pawn();
	void a();
};

