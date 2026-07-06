/*
	SF Chess, a Chess GUI which supports many chess variants
	Copyright (C) 2026  demoman2 (https://github.com/demoman2)

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Affero General Public License as
	published by the Free Software Foundation, either version 3 of the
	License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Affero General Public License for more details.

	You should have received a copy of the GNU Affero General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once
#include <SFML/Graphics.hpp>
#include "Variant.h"
#include "ChessUtil.h"
#include "MoveSet.h"
#include "Region.h"

class Piece
{

public:
	char id, demotedID;
	bool hasMoved, canMove, promoted, promotedPieceType, inCheck;
	Chess::PColor color;

	Piece(char id, int x, int y, float scale, sf::Vector2f boardOffset, sf::Vector2f boardSize, sf::Vector2u boardSquares, float boardMultiplier, Chess::PColor color, const sf::Texture& texture, bool animated, bool promoted, bool promotedPieceType, bool reversed);
	Piece(const Piece& other, bool copySprite = false, bool copySquares = true, bool copyPromotionTypes = true);

	bool operator==(const Piece& other) const;

	// Functions
	~Piece();
	std::shared_ptr<Piece> clone(bool copySprite = false, bool copySquares = true, bool copyPromotionTypes = true) const;

	void addAttacker(const Chess::Variant& variant, const Chess::Position& position) {
		if (variant.checksEnabled && Chess::isRoyal(this, variant, position.extinctionRoyalPieces)) {
			inCheck = true;
		}
	};

	void generateLegalMoves(const Chess::Position& pos, const Chess::Variant& variant);
	bool validatePosition(const Chess::Position& pos, const Chess::Variant& variant) const;

	// Update
	void updateScale(float scale, sf::Vector2f boardOffset, sf::Vector2f boardSize, sf::Vector2u boardSquares, float boardMultiplier, float reversed);
	void updatePosition(float moveSpeed, float deltaTime);
	void updatePosition(float moveSpeed, float captureThreshold, float deltaTime);


	// Utility
	bool isWhite() const { return color == Chess::PColor::White; }
	bool isBlack() const { return color == Chess::PColor::Black; }
	bool contains(sf::Vector2i pos) const { return sprite ? sprite->getGlobalBounds().contains((sf::Vector2f)pos) : false; }
	bool contains(sf::Vector2f pos) const { return sprite ? sprite->getGlobalBounds().contains(pos) : false; }
	bool hasTarget() const { return targetPos.has_value(); }
	bool hasAnimationTarget() const { return animationTarget.has_value(); }
	bool hasID(char c) const { return id == c; }
	sf::Vector2f getTarget() const { return targetPos.value(); }
	const sf::Texture& getTexture() const { return sprite->getTexture(); }
	sf::Vector2i getLocalPos() const { return position; };
	sf::Vector2f getGlobalPos() const { return sprite ? sprite->getPosition() : sf::Vector2f(); };
	bool hasMoveIntersecting(sf::Vector2f position) const;
	const Chess::Square& getMoveIntersecting(sf::Vector2f position) const;
	std::vector<Chess::Square>& getMoveSquares() { return availablePositions; }
	std::vector<Chess::Square>& getCaptureSquares() { return availableCapturePositions; }
	std::vector<Chess::Square>& getEnPassantSquares() { return enPassantPositions; }
	std::vector<Chess::Square>& getCastleSquares() { return castlePositions; }
	std::vector<Chess::Square>& getCaptureCastleSquares() { return captureCastlePositions; }
	std::vector<std::vector<Chess::Square>*>& getPositionVectors() { return positionVectors; }
 
	// Main Sprite
	void makeSprite(const sf::Texture& texture, float scale, sf::Vector2f boardOffset, sf::Vector2f boardSize, sf::Vector2u boardSquares, float boardMultiplier, bool reversed);
	void setTexture(const sf::Texture& texture, float pieceScale);
	void setLocalPosition(sf::Vector2i pos);
	void reversePosition(sf::Vector2f boardOffset, sf::Vector2f boardSize);
	void setPosition(sf::Vector2f pos);
	void setPosition(sf::Vector2i pos);
	void setVisible(bool visible);
	void setTarget(std::optional<sf::Vector2f> target) { targetPos = target; };
	void move(float x, float y) {
		if (sprite) { sprite->move({ x, y }); }
		if (targetPos.has_value()) { targetPos.value() += sf::Vector2f{x, y}; }
		if (animationTarget.has_value()) { animationTarget.value() += sf::Vector2f{ x, y }; }
	}
	void draw(sf::RenderWindow& window) const {
		if (sprite) { window.draw(*sprite); }
	}

	// Square Sprites
	void setSprites(const sf::Texture& texture, sf::Vector2f boardOffset, sf::Vector2f boardSize, sf::Vector2u boardSquares, float boardMultiplier, float pieceScale, bool reversed);
	void updateSprites(const std::vector<sf::Texture>& boardTextures, sf::Vector2f mousePos, bool& mouseSelecting);
	void drawSprites(sf::RenderWindow& window);

private:
	int value;
	std::unique_ptr<sf::Sprite> sprite;
	std::optional<sf::Vector2f> targetPos, animationTarget;
	std::vector<Chess::Square> availablePositions, availableCapturePositions, enPassantPositions;
	std::vector<std::vector<Chess::Square>*> positionVectors;
	sf::Vector2i position;

	std::vector<Chess::Square> castlePositions, captureCastlePositions;

	std::pair<bool, bool> calculateCastlingRights(const Chess::Position& pos, const Chess::Variant& variant);
	void addCastleSquare(sf::Vector2i square);
	void addCastleCaptureSquare(sf::Vector2i square, bool chess960, bool kingside, const Chess::Variant& variant);

	bool addMove(sf::Vector2i square, const Chess::Position& pos, const Chess::Variant& variant);
	bool addCapture(sf::Vector2i square, const Chess::Position& pos, const Chess::Variant& variant);
	bool isValidPassant(sf::Vector2i square, const Chess::Position& pos, const Chess::Variant& variant) const;

	bool isValidMovePos(const Chess::Position& pos, const Chess::Position& newPos, const Chess::Variant& variant, sf::Vector2i square, Chess::PColor color) const;
	bool isValidCapturePos(const Chess::Position& pos, const Chess::Position& newPos, const Chess::Variant& variant, sf::Vector2i square, Chess::PColor color) const;

	void validatePromotionLimit(Chess::PieceType& validTypes, const Chess::Variant& variant, const Chess::Position& pos) const;

	void addMoveSquare(sf::Vector2i square);
	void addCaptureSquare(sf::Vector2i square);
	void addEnPassantSquare(sf::Vector2i square);
};