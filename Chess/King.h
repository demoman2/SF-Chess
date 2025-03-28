#pragma once
#include "Piece.h"

class King : public Piece
{
	bool canCastleK, canCastleQ, hasMoved, inCheck;
public:
	King(int x, int y, float scale, float xOffset, float boardMultiplier, PColor color, sf::Texture& texture, std::vector<std::unique_ptr<Piece>>& pieces);
	King(bool canCastleK, bool canCastleQ, bool hasMoved, int x, int y, float scale, float xOffset, float boardMultiplier, PColor color, sf::Texture& texture, std::vector<std::unique_ptr<Piece>>& pieces);
	void calculatePositions();
};

