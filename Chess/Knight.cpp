#include "Knight.h"

Knight::Knight(int x, int y, float scale, sf::Vector2f boardOffset, float boardMultiplier, PColor color, sf::Texture& texture, bool animated)
    : Piece(x, y, scale, boardOffset, boardMultiplier, color, texture, animated)
{
    name = "Knight";
    id = 'n';
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
