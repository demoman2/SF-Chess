#pragma once
#include "Piece.h"

class King : public virtual Piece
{
public:
	bool canCastleK, canCastleQ, hasMoved, inCheck;
	bool canNeverCastleK, canNeverCastleQ;
	std::vector<sf::Vector2i> castlePositions, captureCastlePositions;
	std::vector<sf::Sprite> castleSquares, castleCaptureSquares;
	King(int x, int y, float scale, float boardOffset, float boardMultiplier, int index, PColor color, sf::Texture& texture);
	King(bool canNeverCastleK, bool canNeverCastleQ, int x, int y, float scale, float boardOffset, float boardMultiplier, int index, PColor color, sf::Texture& texture);
	~King();
};

