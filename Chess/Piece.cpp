#include "Piece.h"
#include "Main.h"

Piece::Piece(int x, int y, float scale, float boardOffset, float boardMultiplier, size_t index, PColor color, sf::Texture& texture, bool animated)
	: sprite(texture), position(x, y), color(color), x(x), y(y), scale(scale), boardOffset(boardOffset), boardMultiplier(boardMultiplier), texture(texture), ghostSprite(texture),
	index(index), hasMoved(false), canMove(false)
{
	if (animated) {
		sprite.setPosition(sf::Vector2f{ boardOffset + ((4.5f - 0.5f) * boardMultiplier), (reverseY(4.5f) - 0.5f) * boardMultiplier });
		animationTarget = sf::Vector2f{ boardOffset + ((x - 0.5f) * boardMultiplier), (reverseY(y) - 0.5f) * boardMultiplier };
	}
	else {
		sprite.setPosition(sf::Vector2f{ boardOffset + ((x - 0.5f) * boardMultiplier), (reverseY(y) - 0.5f) * boardMultiplier });
	}
	sprite.setOrigin(sprite.getLocalBounds().getCenter());
	sprite.setScale(sf::Vector2f(scale, scale));
	ghostSprite.setOrigin(sprite.getLocalBounds().getCenter());
	ghostSprite.setPosition(sprite.getPosition());
	ghostSprite.setScale(sf::Vector2f(scale, scale));
	ghostSprite.setColor(sf::Color(255, 255, 255, 0));
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

void Piece::setGlobalPosition(sf::Vector2f pos)
{
	ghostSprite.setPosition(Main::getGlobalPosition(getLocalPosition(), boardOffset, boardMultiplier));
	sprite.setPosition(pos);
}
