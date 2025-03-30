#include "Pawn.h"

Pawn::Pawn(int x, int y, float scale, float xOffset, float boardMultiplier, PColor color, sf::Texture& texture)
    : Piece(x, y, scale, xOffset, boardMultiplier, color, texture)
{
    name = "Pawn";
    enPassantTarget = false;
    whiteIdentifier = "♙";
    blackIdentifier = "♟";
    pointValue = 1;
    calculatePositions();
}

Pawn::Pawn(bool enPassantTarget, int x, int y, float scale, float xOffset, float boardMultiplier, PColor color, sf::Texture& texture) :
    Piece(x, y, scale, xOffset, boardMultiplier, color, texture)
{
    name = "Pawn";
    this->enPassantTarget = enPassantTarget;
    whiteIdentifier = "♙";
    blackIdentifier = "♟";
    pointValue = 1;
    calculatePositions();
}

void Pawn::calculatePositions()
{

}
