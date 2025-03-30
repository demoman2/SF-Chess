#include "Rook.h"

Rook::Rook(int x, int y, float scale, float xOffset, float boardMultiplier, PColor color, sf::Texture& texture)
    : Piece(x, y, scale, xOffset, boardMultiplier, color, texture), hasMoved(false)
{
    name = "Rook";
    whiteIdentifier = "♖";
    blackIdentifier = "♜";
    pointValue = 5;
}

void Rook::calculatePositions()
{

}
