#include "Knight.h"

Knight::Knight(int x, int y, float scale, float xOffset, float boardMultiplier, PColor color, sf::Texture& texture, std::vector<std::unique_ptr<Piece>>& pieces)
    : Piece(x, y, scale, xOffset, boardMultiplier, color, texture, pieces)
{
    name = "Knight";
    whiteIdentifier = "♘";
    blackIdentifier = "♞";
    pointValue = 3;
    calculatePositions();
}

void Knight::calculatePositions()
{

}
