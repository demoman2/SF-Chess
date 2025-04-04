#include "Piece.h"

Piece::Piece(int x, int y, float scale, float boardOffset, float boardMultiplier, int index, PColor color, sf::Texture& texture)
	: sprite(texture), position(x, y), color(color), x(x), y(y), scale(scale), boardOffset(boardOffset), boardMultiplier(boardMultiplier), texture(texture), index(index)
{
	sprite.setPosition(sf::Vector2f{ boardOffset + ((x - 1) * boardMultiplier), (reverseY(y) - 1) * boardMultiplier});
	sprite.setScale(sf::Vector2f(scale, scale));
	allSquares.push_back(selectionSquares);
	allSquares.push_back(captureSquares);
	allPositions.push_back(availablePositions);
	allPositions.push_back(availableCapturePositions);
}

Piece::~Piece()
{

}

void Piece::move(int newX, int newY)
{
	x = newX;
	y = newY;
	sprite.setPosition(sf::Vector2f{ newX * boardMultiplier, newY * boardMultiplier });
}

std::string Piece::getIdentifier() const
{
	if (isWhite()) {
		return whiteIdentifier;
	}
	return blackIdentifier;
}