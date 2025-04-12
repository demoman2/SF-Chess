#include "Knight.h"

Knight::Knight(int x, int y, float scale, float boardOffset, float boardMultiplier, size_t index, PColor color, sf::Texture& texture, bool animated)
    : Piece(x, y, scale, boardOffset, boardMultiplier, index, color, texture, animated)
{
    name = "Knight";
    whiteIdentifier = "♘";
    blackIdentifier = "♞";
    pointValue = 3;

}

Knight::~Knight()
{
}

std::shared_ptr<Piece> Knight::clone() const
{
    return std::make_shared<Knight>(*this);
}
