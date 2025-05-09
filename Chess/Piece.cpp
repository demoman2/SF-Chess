#include "Piece.h"
#include "Board.h"
#include "Main.h"

Piece::Piece(int x, int y, float scale, sf::Vector2f boardOffset, float boardMultiplier, PColor color, sf::Texture& texture, bool animated)
	: sprite(texture), position(x, y), color(color), x(x), y(y), texture(texture), ghostSprite(texture),
	hasMoved(false), canMove(false), promoted(false), pointValue(0)
{
	if (animated) {
		sprite.setPosition(Main::getGlobalPositionF(sf::Vector2f{ 4.5f, 4.5f }, boardOffset, boardMultiplier));
		animationTarget = Main::getGlobalPosition({x, y}, boardOffset, boardMultiplier);
	}
	else {
		sprite.setPosition(Main::getGlobalPosition({ x, y }, boardOffset, boardMultiplier));
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
	if (pieceVisible) { sprite.setColor(sf::Color(255, 255, 255, 255)); }
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

void Piece::setGlobalPosition(sf::Vector2f pos, sf::Vector2f boardOffset, float boardMultiplier)
{
	if (this != nullptr) {
		ghostSprite.setPosition(Main::getGlobalPosition(getLocalPosition(), boardOffset, boardMultiplier));
		sprite.setPosition(pos);
	}
}

void Piece::setPosition(sf::Vector2f pos)
{
	if (this != nullptr) {
		ghostSprite.setPosition(pos);
		sprite.setPosition(pos);
	}
}

void Piece::setPosition(sf::Vector2i pos)
{
	if (this != nullptr) {
		ghostSprite.setPosition((sf::Vector2f)pos);
		sprite.setPosition((sf::Vector2f)pos);
	}
}
