#include "Queen.h"

Queen::Queen(int x, int y, float scale, float boardOffset, float boardMultiplier, int index, PColor color, sf::Texture& texture)
    : Piece(x, y, scale, boardOffset, boardMultiplier, index, color, texture)
{
    name = "Queen";
    whiteIdentifier = "♕";
    blackIdentifier = "♛";
    pointValue = 9;
}

Queen::~Queen()
{
}