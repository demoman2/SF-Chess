#include "King.h"

King::King(int x, int y, float scale, float xOffset, float boardMultiplier, PColor color, sf::Texture& texture, std::vector<std::unique_ptr<Piece>>& pieces)
    : Piece(x, y, scale, xOffset, boardMultiplier, color, texture, pieces)
{
    name = "King";
    canCastleK = false;
    canCastleQ = false;
    whiteIdentifier = "♔";
    blackIdentifier = "♚";
    pointValue = 10000;
    calculatePositions();
}

King::King(bool canCastleK, bool canCastleQ, bool hasMoved, int x, int y, float scale, float xOffset, float boardMultiplier, PColor color, sf::Texture& texture, std::vector<std::unique_ptr<Piece>>& pieces)
    : Piece(x, y, scale, xOffset, boardMultiplier, color, texture, pieces)
{
    name = "King";
    this->canCastleK = canCastleK;
    this->canCastleQ = canCastleQ;
    this->hasMoved = hasMoved;
    whiteIdentifier = "♔";
    blackIdentifier = "♚";
    pointValue = 10000;
    calculatePositions();
}

void King::calculatePositions()
{

}
