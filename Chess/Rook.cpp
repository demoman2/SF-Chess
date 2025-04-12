#include "Rook.h"

Rook::Rook(int x, int y, float scale, float boardOffset, float boardMultiplier, size_t index, PColor color, sf::Texture& texture, bool animated)
    : Piece(x, y, scale, boardOffset, boardMultiplier, index, color, texture, animated)
{
    name = "Rook";
    whiteIdentifier = "♖";
    blackIdentifier = "♜";
    pointValue = 5;
}

Rook::~Rook()
{
}

std::shared_ptr<Piece> Rook::clone() const
{
    return std::make_shared<Rook>(*this);
}
