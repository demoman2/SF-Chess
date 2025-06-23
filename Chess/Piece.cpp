#include "Piece.h"
#include <iostream>

Piece::Piece(int x, int y, float scale, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, Chess::PColor color, sf::Texture& texture, bool animated, bool promoted, bool reversed)
	: sprite(texture), position(x, y), color(color), texture(texture), id(' '), hasMoved(false), canMove(false), promoted(promoted), pointValue(0),
	availablePositions(std::make_shared<std::vector<Chess::Square>>()), availableCapturePositions(std::make_shared<std::vector<Chess::Square>>())
{
	positionVectors.push_back(availablePositions);
	positionVectors.push_back(availableCapturePositions);
	if (animated) {
		sprite.setPosition(Chess::getGlobalPosition(sf::Vector2f{ 4.5f, 4.5f }, boardOffset, boardSize, boardMultiplier, reversed));
		animationTarget = Chess::getGlobalPosition(sf::Vector2i{ x, y }, boardOffset, boardSize, boardMultiplier, reversed);
	}
	else {
		sprite.setPosition(Chess::getGlobalPosition(sf::Vector2i{ x, y }, boardOffset, boardSize, boardMultiplier, reversed));
	}
	sprite.setOrigin(sprite.getLocalBounds().getCenter());
	sprite.setScale(sf::Vector2f(scale, scale));
}

Piece::~Piece()
{
}

void Piece::setLocalPosition(sf::Vector2i pos)
{
	position = { pos };
}

void Piece::reversePosition(sf::Vector2f boardOffset, sf::Vector2f boardSize)
{
	setPosition(Chess::reversePosition(getGlobalPos(), boardSize) + boardOffset + boardOffset);
}

void Piece::setPosition(sf::Vector2f pos)
{
	sprite.setPosition(pos);
}

void Piece::setPosition(sf::Vector2i pos)
{
	sprite.setPosition((sf::Vector2f)pos);
}

void Piece::addMoveSquare(sf::Vector2i square)
{
	std::string m = Chess::convertPositiontoNotation(position) + Chess::convertPositiontoNotation(square);
	availablePositions->emplace_back(square, m);
}

void Piece::addCaptureSquare(sf::Vector2i square)
{
	std::string m = Chess::convertPositiontoNotation(position) + Chess::convertPositiontoNotation(square);
	availableCapturePositions->emplace_back(square, m);
}

void Piece::setVisible(bool visible)
{
	setSpriteVisible(sprite, visible);
}

void Piece::updateScale(float scale, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, float reversed)
{
	sprite.setScale(sf::Vector2f(scale, scale));
	sprite.setPosition(Chess::getGlobalPosition(position, boardOffset, boardSize, boardMultiplier, reversed));
}

bool Piece::isValidMove(sf::Vector2i square, const pieceVector& pieceList, Chess::Variant variant, bool atomicKings, bool checksEnabled)
{
	if (checksEnabled) {
		if (!atomicKings) {
			pieceVector newV = Chess::makePieceVec(pieceList, square, getLocalPos(), variant);
			if (variant == Chess::RacingKings) {
				if (!isValidPosition(newV, color) || !isValidPosition(newV, !color)) {
					return false;
				}
			}
			else {
				if (!isValidPosition(newV, color)) {
					return false;
				}
			}
		}
	}
	return true;
}

bool Piece::isValidCapture(sf::Vector2i square, const pieceVector& pieceList, Chess::Variant variant, bool atomicKings, bool checksEnabled)
{
	if (checksEnabled) {
		if (!atomicKings) {
			pieceVector newV = Chess::makePieceVec(pieceList, square, getLocalPos(), variant);
			if (variant == Chess::Atomic) {
				if (!isValidPosition(newV, color)) {
					if (!(canCaptureEnemyKing(square, pieceList, color) && isValidAtomicCapture(square, pieceList, color))) {
						return false;
					}
				}
				else if (!isValidAtomicCapture(square, pieceList, color)) {
					return false;
				}

			}
			else if (variant == Chess::RacingKings) {
				if (!isValidPosition(newV, color) || !isValidPosition(newV, !color)) {
					return false;
				}
			}
			else {
				if (!isValidPosition(newV, color) && !(Chess::getPieceFromPosition(square, pieceList)->IsA("King") && Chess::getPieceFromPosition(square, pieceList)->color != color)) {
					return false;
				}
			}
		}
		else {
			if (!isValidAtomicCapture(square, pieceList, color)) {
				return false;
			}
		}
	}
	return true;
}

void Piece::setSprites(sf::Texture& texture, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, float pieceScale, bool reversed)
{
	for (auto& vec : positionVectors) {
		for (auto& square : *vec) {
			square.setupSprite(texture, boardOffset, boardSize, boardMultiplier, pieceScale, reversed);
		}
	}
}

void Piece::updateSprites(std::vector<sf::Texture>& boardTextures, sf::Vector2f mousePos, bool& mouseSelecting)
{
	for (auto& square : *availablePositions) {
		if (square.sprite.has_value()) {
			auto& sprite = square.sprite.value();
			if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
				mouseSelecting = true;
				sprite.setTexture(boardTextures.at(4), true);
			}
			else { sprite.setTexture(boardTextures.at(0), true); }
		}
	}
	for (auto& square : *availableCapturePositions) {
		if (square.sprite.has_value()) {
			auto& sprite = square.sprite.value();
			if (sprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
				mouseSelecting = true;
				sprite.setTexture(boardTextures.at(4));
			}
			else { sprite.setTexture(boardTextures.at(1)); }
		}
	}
}

void Piece::drawSprites(sf::RenderWindow& window)
{
	for (auto& vec : positionVectors) {
		for (auto& square : *vec) {
			std::optional<sf::Sprite> sprite = square.sprite;
			if (sprite.has_value()) {
				window.draw(sprite.value());
			}
		}
	}
}

void Piece::updatePosition()
{
	if (animationTarget.has_value()) {
		if (sprite.getPosition() != animationTarget.value()) {
			sprite.setPosition(Interpolate(sprite.getPosition(), animationTarget.value(), 0.22f));
		}
		else {
			animationTarget = {};
		}
	}
	else if (targetPos.has_value()) {
		sprite.setPosition(Interpolate(sprite.getPosition(), targetPos.value(), 0.25f));
	}
}

void Piece::updatePosition(float captureThreshold)
{
	if (targetPos.has_value()) {
		sprite.setPosition(Interpolate(sprite.getPosition(), targetPos.value(), 0.25f, captureThreshold));
	}
}

std::string Piece::getMoveIntersecting(sf::Vector2f position)
{
	for (auto& vec : positionVectors) {
		for (auto& square : *vec) {
			if (square.sprite.has_value() && square.sprite.value().getGlobalBounds().contains(position)) {
				return square.moveString;
			}
		}
	}
	return "";
}