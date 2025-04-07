#include "Bishop.h"

Bishop::Bishop(int x, int y, float scale, float boardOffset, float boardMultiplier, size_t index, PColor color, sf::Texture& texture)
	: Piece(x, y, scale, boardOffset, boardMultiplier, index, color, texture)
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