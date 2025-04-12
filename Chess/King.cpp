#include "King.h"

King::King(int x, int y, float scale, float boardOffset, float boardMultiplier, size_t index, PColor color, sf::Texture& texture, bool animated)
    : Piece(x, y, scale, boardOffset, boardMultiplier, index, color, texture, animated), canCastleK(false), canCastleQ(false), canNeverCastleK(false), canNeverCastleQ(false), inCheck(false)
{
    name = "King";
    whiteIdentifier = "♔";
    blackIdentifier = "♚";
    pointValue = 10000;
}

King::King(bool canNeverCastleK, bool canNeverCastleQ, int x, int y, float scale, float boardOffset, float boardMultiplier, size_t index, PColor color, sf::Texture& texture, bool animated)
    : Piece(x, y, scale, boardOffset, boardMultiplier, index, color, texture, animated), canCastleK(false), canCastleQ(false), inCheck(false)
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

std::shared_ptr<Piece> King::clone() const
{
    return std::make_shared<King>(*this);
}

