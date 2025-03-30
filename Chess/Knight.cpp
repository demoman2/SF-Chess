#include "Knight.h"

Knight::Knight(int x, int y, float scale, float xOffset, float boardMultiplier, PColor color, sf::Texture& texture)
    : Piece(x, y, scale, xOffset, boardMultiplier, color, texture)
{
    name = "Knight";
    whiteIdentifier = "♘";
    blackIdentifier = "♞";
    pointValue = 3;
    calculatePositions();
}

void Knight::calculatePositions()
{

}
