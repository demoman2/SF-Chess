#include "Bishop.h"

Bishop::Bishop(int x, int y, float scale, float xOffset, float boardMultiplier, PColor color, sf::Texture& texture)
	: Piece(x, y, scale, xOffset, boardMultiplier, color, texture)
{
    name = "Bishop";
    whiteIdentifier = "♗";
    blackIdentifier = "♝";
    pointValue = 3;
    calculatePositions();
}

void Bishop::calculatePositions() 
{

}
