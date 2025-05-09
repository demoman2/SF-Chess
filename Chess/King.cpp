#include "King.h"

King::King(int x, int y, float scale, sf::Vector2f boardOffset, float boardMultiplier, PColor color, sf::Texture& texture, bool animated)
    : Piece(x, y, scale, boardOffset, boardMultiplier, color, texture, animated), canCastleK(false), canCastleQ(false), canNeverCastleK(false), canNeverCastleQ(false), inCheck(false)
{
    name = "King";
    id = 'k';
    whiteIdentifier = "♔";
    blackIdentifier = "♚";
    pointValue = 10000;
}

King::King(bool canNeverCastleK, bool canNeverCastleQ, int x, int y, float scale, sf::Vector2f boardOffset, float boardMultiplier, PColor color, sf::Texture& texture, bool animated)
    : Piece(x, y, scale, boardOffset, boardMultiplier, color, texture, animated), canCastleK(false), canCastleQ(false), inCheck(false)
{
    name = "King";
    id = 'k';
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

