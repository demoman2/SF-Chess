#include "King.h"

King::King(int x, int y, float scale, float xOffset, float boardMultiplier, int index, PColor color, sf::Texture& texture)
    : Piece(x, y, scale, xOffset, boardMultiplier, index, color, texture), hasMoved(false)
{
    name = "King";
    canCastleK = false;
    canCastleQ = false;
    whiteIdentifier = "♔";
    blackIdentifier = "♚";
    pointValue = 10000;
}

King::King(bool canNeverCastleK, bool canNeverCastleQ, int x, int y, float scale, float xOffset, float boardMultiplier, int index, PColor color, sf::Texture& texture)
    : Piece(x, y, scale, xOffset, boardMultiplier, index, color, texture), hasMoved(false)
{
    name = "King";
    this->canNeverCastleK = canNeverCastleK;
    this->canNeverCastleQ = canNeverCastleQ;
    whiteIdentifier = "♔";
    blackIdentifier = "♚";
    pointValue = 10000;
}

King::~King()
{
}