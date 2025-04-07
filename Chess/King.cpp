#include "King.h"

King::King(int x, int y, float scale, float boardOffset, float boardMultiplier, size_t index, PColor color, sf::Texture& texture)
    : Piece(x, y, scale, boardOffset, boardMultiplier, index, color, texture), hasMoved(false), canCastleK(false), canCastleQ(false)
{
    name = "King";
    canCastleK = false;
    canCastleQ = false;
    whiteIdentifier = "♔";
    blackIdentifier = "♚";
    pointValue = 10000;
    allSquares.push_back(castleSquares);
    allSquares.push_back(castleCaptureSquares);
    allPositions.push_back(castlePositions);
    allPositions.push_back(captureCastlePositions);
}

King::King(bool canNeverCastleK, bool canNeverCastleQ, int x, int y, float scale, float boardOffset, float boardMultiplier, size_t index, PColor color, sf::Texture& texture)
    : Piece(x, y, scale, boardOffset, boardMultiplier, index, color, texture), hasMoved(false), canCastleK(false), canCastleQ(false)
{
    name = "King";
    this->canNeverCastleK = canNeverCastleK;
    this->canNeverCastleQ = canNeverCastleQ;
    whiteIdentifier = "♔";
    blackIdentifier = "♚";
    pointValue = 10000;
    allSquares.push_back(castleSquares);
    allSquares.push_back(castleCaptureSquares);
    allPositions.push_back(castlePositions);
    allPositions.push_back(captureCastlePositions);
}

King::~King()
{
}

std::shared_ptr<Piece> King::clone() const
{
    return std::make_shared<King>(*this);
}

