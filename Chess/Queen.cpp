#include "Queen.h"

Queen::Queen(int x, int y, float scale, float xOffset, float boardMultiplier, int index, PColor color, sf::Texture& texture)
    : Piece(x, y, scale, xOffset, boardMultiplier, index, color, texture)
{
    name = "Queen";
    whiteIdentifier = "♕";
    blackIdentifier = "♛";
    pointValue = 9;
}

Queen::~Queen()
{
}

void Queen::a()
{
}
