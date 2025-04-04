#pragma once
#include "Piece.h"

class Pawn : public virtual Piece
{

public:
	bool enPassantTarget, hasMoved;
	std::vector<sf::Vector2i> enPassantPositions;
	std::vector<sf::Sprite> enPassantSquares;
	Pawn(int x, int y, float scale, float boardOffset, float boardMultiplier, int index, PColor color, sf::Texture& texture);
	Pawn(bool enPassantTarget, int x, int y, float scale, float boardOffset, float boardMultiplier, int index, PColor color, sf::Texture& texture);
	~Pawn();
};

