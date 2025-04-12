#include "Queen.h"

Queen::Queen(int x, int y, float scale, float boardOffset, float boardMultiplier, size_t index, PColor color, sf::Texture& texture, bool animated)
    : Piece(x, y, scale, boardOffset, boardMultiplier, index, color, texture, animated)
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