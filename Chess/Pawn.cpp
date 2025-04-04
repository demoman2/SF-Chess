#include "Pawn.h"

Pawn::Pawn(int x, int y, float scale, float boardOffset, float boardMultiplier, int index, PColor color, sf::Texture& texture)
    : Piece(x, y, scale, boardOffset, boardMultiplier, index, color, texture), hasMoved(true)
{
    name = "Pawn";
    enPassantTarget = false;
    whiteIdentifier = "♙";
    blackIdentifier = "♟";
    pointValue = 1;
    if (color == Piece::PColor::White) {
        if (y == 2) {
            hasMoved = false;
        }
    }
    else {
        if (y == 7) {
            hasMoved = false;
        }
    }
    allSquares.push_back(enPassantSquares);
    allPositions.push_back(enPassantPositions);
}

Pawn::Pawn(bool enPassantTarget, int x, int y, float scale, float boardOffset, float boardMultiplier, int index, PColor color, sf::Texture& texture) :
    Piece(x, y, scale, boardOffset, boardMultiplier, index, color, texture), hasMoved(true)
{
    name = "Pawn";
    this->enPassantTarget = enPassantTarget;
    whiteIdentifier = "♙";
    blackIdentifier = "♟";
    pointValue = 1;
    if (color == Piece::PColor::White) {
        if (y == 2) {
            hasMoved = false;
        }
    }
    else {
        if (y == 7) {
            hasMoved = false;
        }
    }
    allSquares.push_back(enPassantSquares);
    allPositions.push_back(enPassantPositions);
}

Pawn::~Pawn()
{
}