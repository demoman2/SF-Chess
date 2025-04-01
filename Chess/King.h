#pragma once
#include "Piece.h"

class King : public virtual Piece
{
public:
	bool canCastleK, canCastleQ, hasMoved, inCheck;
	bool canNeverCastleK, canNeverCastleQ;
	King(int x, int y, float scale, float xOffset, float boardMultiplier, int index, PColor color, sf::Texture& texture);
	King(bool canNeverCastleK, bool canNeverCastleQ, int x, int y, float scale, float xOffset, float boardMultiplier, int index, PColor color, sf::Texture& texture);
	~King();
	void a();
};

