#include "Piece.h"

Piece::Piece(int x, int y, float scale, float xOffset, float boardMultiplier, int index, PColor color, sf::Texture& texture)
	: sprite(texture), position(x, y), color(color), x(x), y(y), scale(scale), xOffset(xOffset), boardMultiplier(boardMultiplier), texture(texture), index(index)
{
	sprite.setPosition(sf::Vector2f{ xOffset + ((x - 1) * boardMultiplier), (reverseY(y) - 1) * boardMultiplier});
	sprite.setScale(sf::Vector2f(scale, scale));

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