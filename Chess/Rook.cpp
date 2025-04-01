#include "Rook.h"

Rook::Rook(int x, int y, float scale, float xOffset, float boardMultiplier, int index, PColor color, sf::Texture& texture)
    : Piece(x, y, scale, xOffset, boardMultiplier, index, color, texture), hasMoved(false)
{
    name = "Rook";
    whiteIdentifier = "♖";
    blackIdentifier = "♜";
    pointValue = 5;
}

Rook::~Rook()
{
}

void Rook::a()
{
}
