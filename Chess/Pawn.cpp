#include "Pawn.h"

Pawn::Pawn(int x, int y, float scale, float xOffset, float boardMultiplier, PColor color, sf::Texture& texture, std::vector<std::unique_ptr<Piece>>& pieces)
    : Piece(x, y, scale, xOffset, boardMultiplier, color, texture, pieces)
{
    name = "Pawn";
    hasMoved = false;
    enPassantTarget = false;
    whiteIdentifier = "♙";
    blackIdentifier = "♟";
    pointValue = 1;
    calculatePositions();
}

Pawn::Pawn(bool hasMoved, bool enPassantTarget, int x, int y, float scale, float xOffset, float boardMultiplier, PColor color, sf::Texture& texture, std::vector<std::unique_ptr<Piece>>& pieces) :
    Piece(x, y, scale, xOffset, boardMultiplier, color, texture, pieces)
{
    name = "Pawn";
    this->hasMoved = hasMoved;
    this->enPassantTarget = enPassantTarget;
    whiteIdentifier = "♙";
    blackIdentifier = "♟";
    pointValue = 1;
    calculatePositions();
}

void Pawn::calculatePositions()
{

}
