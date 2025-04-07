#include "Piece.h"

Piece::Piece(int x, int y, float scale, float boardOffset, float boardMultiplier, size_t index, PColor color, sf::Texture& texture)
	: sprite(texture), position(x, y), color(color), x(x), y(y), scale(scale), boardOffset(boardOffset), boardMultiplier(boardMultiplier), texture(texture), ghostSprite(texture),
	index(index), moving(false), captured(false)
{
	sprite.setOrigin(sprite.getLocalBounds().getCenter());
	sprite.setPosition(sf::Vector2f{ boardOffset + ((x - 0.5f) * boardMultiplier), (reverseY(y) - 0.5f) * boardMultiplier});
	sprite.setScale(sf::Vector2f(scale, scale));
	ghostSprite.setOrigin(sprite.getLocalBounds().getCenter());
	ghostSprite.setPosition(sf::Vector2f{ boardOffset + ((x - 0.5f) * boardMultiplier), (reverseY(y) - 0.5f) * boardMultiplier });
	ghostSprite.setScale(sf::Vector2f(scale, scale));
	ghostSprite.setColor(sf::Color(0, 0, 0, 0));
	allSquares.push_back(selectionSquares);
	allSquares.push_back(captureSquares);
	allPositions.push_back(availablePositions);
	allPositions.push_back(availableCapturePositions);
}

Piece::~Piece()
{
	
}

std::string Piece::getIdentifier() const
{
	if (isWhite()) {
		return whiteIdentifier;
	}
	return blackIdentifier;
}

void Piece::setGhostSpriteVisible(bool visible, bool pieceVisible)
{
	if (visible) { ghostSprite.setColor(ghostSpriteColor); }
	else {
		ghostSprite.setColor(sf::Color(255, 255, 255, 0));
	}
	if (pieceVisible) {
		sprite.setColor(sf::Color(255, 255, 255, 255));
	}
	else {
		sprite.setColor(sf::Color(255, 255, 255, 0));
	}
}

void Piece::setLocalPosition(sf::Vector2i pos)
{
	this->x = pos.x;
	this->y = pos.y;
	position = { pos };
}

void Piece::setGlobalPosition(sf::Vector2f pos, bool ghost)
{
	if (ghost) {
		ghostSprite.setPosition(pos);
	}
	sprite.setPosition(pos);
}
