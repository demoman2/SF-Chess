#pragma once
#include "Piece.h"

class King : public virtual Piece
{
public:
	bool canCastleK, canCastleQ, inCheck;
	bool canNeverCastleK, canNeverCastleQ;
	std::vector<sf::Vector2i> castlePositions, captureCastlePositions;
	std::vector<sf::Sprite> castleSquares, castleCaptureSquares;
	King(int x, int y, float scale, float boardXOffset, float boardMultiplier, PColor color, sf::Texture& texture, bool animated);
	King(bool canNeverCastleK, bool canNeverCastleQ, int x, int y, float scale, float boardXOffset, float boardMultiplier, PColor color, sf::Texture& texture, bool animated);
	~King();
	virtual std::shared_ptr<Piece> clone() const override;
};

