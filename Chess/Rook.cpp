#include "Rook.h"

Rook::Rook(int x, int y, float scale, float xOffset, float boardMultiplier, PColor color, sf::Texture& texture, std::vector<std::unique_ptr<Piece>>& pieces)
    : Piece(x, y, scale, xOffset, boardMultiplier, color, texture, pieces)
{
    name = "Rook";
    if (!kingSide) {
        queenSide = true;
    }
    else { queenSide = false; }
    whiteIdentifier = "♖";
    blackIdentifier = "♜";
    pointValue = 5;
    calculatePositions();
}

Rook::Rook(int x, int y, bool kingSide, bool hasMoved, float scale, float xOffset, float boardMultiplier, PColor color, sf::Texture& texture, std::vector<std::unique_ptr<Piece>>& pieces)
    : Piece(x, y, scale, xOffset, boardMultiplier, color, texture, pieces)
{
    name = "Rook";
    this->hasMoved = hasMoved;
    if (!kingSide) {
        queenSide = true;
    }
    else { queenSide = false; }
    whiteIdentifier = "♖";
    blackIdentifier = "♜";
    pointValue = 5;
    calculatePositions();
}

void Rook::calculatePositions()
{

}
