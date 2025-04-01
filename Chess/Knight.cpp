#include "Knight.h"

Knight::Knight(int x, int y, float scale, float xOffset, float boardMultiplier, int index, PColor color, sf::Texture& texture)
    : Piece(x, y, scale, xOffset, boardMultiplier, index, color, texture)
{
    name = "Knight";
    whiteIdentifier = "♘";
    blackIdentifier = "♞";
    pointValue = 3;

}

Knight::~Knight()
{
}