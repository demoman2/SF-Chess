#include "Queen.h"

Queen::Queen(int x, int y, float scale, float xOffset, float boardMultiplier, PColor color, sf::Texture& texture)
    : Piece(x, y, scale, xOffset, boardMultiplier, color, texture)
{
    name = "Queen";
    whiteIdentifier = "♕";
    blackIdentifier = "♛";
    pointValue = 9;
    calculatePositions();
}

void Queen::calculatePositions()
{

}
