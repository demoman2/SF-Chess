#include "Bishop.h"

Bishop::Bishop(int x, int y, float scale, float boardXOffset, float boardMultiplier, PColor color, sf::Texture& texture, bool animated)
	: Piece(x, y, scale, boardXOffset, boardMultiplier, color, texture, animated)
{
    name = "Bishop";
    whiteIdentifier = "♗";
    blackIdentifier = "♝";
    pointValue = 3;
}

Bishop::~Bishop()
{
}

std::shared_ptr<Piece> Bishop::clone() const
{
    return std::make_shared<Bishop>(*this);
}