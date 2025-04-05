#include "Piece.h"

Piece::Piece(int x, int y, float scale, float boardOffset, float boardMultiplier, int index, PColor color, sf::Texture& texture)
	: sprite(texture), position(x, y), color(color), x(x), y(y), scale(scale), boardOffset(boardOffset), boardMultiplier(boardMultiplier), texture(texture),
	index(index), moving(false)
{
	sprite.setOrigin(sprite.getLocalBounds().getCenter());
	sprite.setPosition(sf::Vector2f{ boardOffset + ((x - 0.5f) * boardMultiplier), (reverseY(y) - 0.5f) * boardMultiplier});
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