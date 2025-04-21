#include "Pawn.h"

Pawn::Pawn(int x, int y, float scale, float boardOffset, float boardMultiplier, PColor color, sf::Texture& texture, bool animated)
    : Piece(x, y, scale, boardOffset, boardMultiplier, color, texture, animated)
{
    hasMoved = true;
    name = "Pawn";
    enPassantTarget = false;
    capturingEnPassant = false;
    whiteIdentifier = "♙";
    blackIdentifier = "♟";
    pointValue = 1;
    if (isWhite()) {
        if (y == 2) {
            hasMoved = false;
        }
    }
    else {
        if (y == 7) {
            hasMoved = false;
        }
    }
}

Pawn::Pawn(bool enPassantTarget, int x, int y, float scale, float boardOffset, float boardMultiplier, PColor color, sf::Texture& texture, bool animated) :
    Piece(x, y, scale, boardOffset, boardMultiplier, color, texture, animated)
{
    hasMoved = true;
    name = "Pawn";
    this->enPassantTarget = enPassantTarget;
    capturingEnPassant = false;
    whiteIdentifier = "♙";
    blackIdentifier = "♟";
    pointValue = 1;
    if (isWhite()) {
        if (y == 2) {
            hasMoved = false;
        }
    }
    else {
        if (y == 7) {
            hasMoved = false;
        }
    }
}

Pawn::~Pawn()
{
}

std::shared_ptr<Piece> Pawn::clone() const
{
    return std::make_shared<Pawn>(*this);
}