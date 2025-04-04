#include "Bishop.h"

Bishop::Bishop(int x, int y, float scale, float boardOffset, float boardMultiplier, int index, PColor color, sf::Texture& texture)
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