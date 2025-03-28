#pragma once
#include "Piece.h"

class Pawn : public Piece
{

public:
	bool hasMoved, enPassantTarget;
	Pawn(int x, int y, float scale, float xOffset, float boardMultiplier, PColor color, sf::Texture& texture, std::vector<std::unique_ptr<Piece>>& pieces);
	Pawn(bool hasMoved, bool enPassantTarget, int x, int y, float scale, float xOffset, float boardMultiplier, PColor color, sf::Texture& texture, std::vector<std::unique_ptr<Piece>>& pieces);
	void calculatePositions();
};

