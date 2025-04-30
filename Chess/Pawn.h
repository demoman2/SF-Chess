#pragma once
#include "Piece.h"

class Pawn : public virtual Piece
{

public:
	bool enPassantTarget, capturingEnPassant;
	std::vector<sf::Vector2i> enPassantPositions;
	std::vector<sf::Sprite> enPassantSquares;
	Pawn(int x, int y, float scale, float boardXOffset, float boardMultiplier, PColor color, sf::Texture& texture, bool animated);
	Pawn(bool enPassantTarget, int x, int y, float scale, float boardXOffset, float boardMultiplier, PColor color, sf::Texture& texture, bool animated);
	~Pawn();
	virtual std::shared_ptr<Piece> clone() const override;
};

