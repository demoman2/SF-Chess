#include "Queen.h"

Queen::Queen(int x, int y, float scale, float boardXOffset, float boardMultiplier, PColor color, sf::Texture& texture, bool animated)
    : Piece(x, y, scale, boardXOffset, boardMultiplier, color, texture, animated)
{
    name = "Queen";
    whiteIdentifier = "♕";
    blackIdentifier = "♛";
    pointValue = 9;
}

Queen::~Queen()
{
}

std::shared_ptr<Piece> Queen::clone() const
{
    return std::make_shared<Queen>(*this);
}