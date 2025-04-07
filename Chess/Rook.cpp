#include "Rook.h"

Rook::Rook(int x, int y, float scale, float boardOffset, float boardMultiplier, size_t index, PColor color, sf::Texture& texture)
    : Piece(x, y, scale, boardOffset, boardMultiplier, index, color, texture), hasMoved(false)
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
