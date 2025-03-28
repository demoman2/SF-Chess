#include "Queen.h"

Queen::Queen(int x, int y, float scale, float xOffset, float boardMultiplier, PColor color, sf::Texture& texture, std::vector<std::unique_ptr<Piece>>& pieces)
    : Piece(x, y, scale, xOffset, boardMultiplier, color, texture, pieces)
{
    name = "Queen";
    whiteIdentifier = "♕";
    blackIdentifier = "♛";
    pointValue = 9;
    calculatePositions();
}

void Queen::calculatePositions()
{

}
