#include "Bishop.h"

Bishop::Bishop(int x, int y, float scale, sf::Vector2f boardOffset, float boardMultiplier, PColor color, sf::Texture& texture, bool animated)
	: Piece(x, y, scale, boardOffset, boardMultiplier, color, texture, animated)
{
    name = "Bishop";
    id = 'b';
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