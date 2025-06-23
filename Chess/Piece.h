#pragma once
#include <SFML/Graphics.hpp>
#include "ChessUtil.h"

class Piece
{

public:
	std::string name;
	char id;
	bool hasMoved, canMove, promoted;
	int pointValue;
	Chess::PColor color;

	Piece(int x, int y, float scale, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, Chess::PColor color, sf::Texture& texture, bool animated, bool promoted, bool reversed);

	// Virtual Functions
	virtual ~Piece();
	virtual std::shared_ptr<Piece> clone() const = 0;

	virtual void addAttacker(){};
	virtual void afterMovePlayed(){};

	virtual void generateLegalMoves(const pieceVector& pieceList, Chess::Variant variant, bool atomicKings, bool checksEnabled, bool castlingEnabled, bool chess960, bool hasDoubleCheck) = 0;
	virtual bool validatePosition(const pieceVector& pieceList) = 0;

	// Update
	void updateScale(float scale, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, float reversed);
	void updatePosition();
	void updatePosition(float captureThreshold);

	// Utility
	bool isWhite() const { return color == Chess::PColor::White; }
	bool isBlack() const { return color == Chess::PColor::Black; }
	bool contains(sf::Vector2i pos) { return sprite.getGlobalBounds().contains((sf::Vector2f)pos); }
	bool contains(sf::Vector2f pos) { return sprite.getGlobalBounds().contains(pos); }
	bool hasTarget() const { return targetPos.has_value(); }
	bool hasAnimationTarget() const { return animationTarget.has_value(); }
	bool IsA(std::string n) const { return name == n; }
	sf::Vector2f getTarget() { return targetPos.value(); }
	const sf::Texture& getTexture() { return sprite.getTexture(); }
	sf::Vector2i getLocalPos() const { return position; };
	sf::Vector2f getGlobalPos() { return sprite.getPosition(); };
	std::string getMoveIntersecting(sf::Vector2f position);
	std::shared_ptr<std::vector<Chess::Square>> getMoveSquares() { return availablePositions; }
	std::shared_ptr<std::vector<Chess::Square>> getCaptureSquares() { return availableCapturePositions; }
 
	// Main Sprite
	void setLocalPosition(sf::Vector2i pos);
	void reversePosition(sf::Vector2f boardOffset, sf::Vector2f boardSize);
	void setPosition(sf::Vector2f pos);
	void setPosition(sf::Vector2i pos);
	void setVisible(bool visible);
	void setTarget(std::optional<sf::Vector2f> target) { targetPos = target; };
	void move(float x, float y) { sprite.move({ x, y }); }
	void draw(sf::RenderWindow& window) { window.draw(sprite); }

	// Square Sprites
	void setSprites(sf::Texture& texture, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, float pieceScale, bool reversed);
	virtual void updateSprites(std::vector<sf::Texture>& boardTextures, sf::Vector2f mousePos, bool& mouseSelecting);
	void drawSprites(sf::RenderWindow& window);

protected:
	sf::Vector2i position;
	std::optional<sf::Vector2f> targetPos = {}, animationTarget = {};
	std::shared_ptr<std::vector<Chess::Square>> availablePositions, availableCapturePositions;
	std::vector<std::shared_ptr<std::vector<Chess::Square>>> positionVectors;
	sf::Sprite sprite;
	sf::Texture texture;

	virtual bool isValidMove(sf::Vector2i square, const pieceVector& pieceList, Chess::Variant variant, bool atomicKings, bool checksEnabled);
	virtual bool isValidCapture(sf::Vector2i square, const pieceVector& pieceList, Chess::Variant variant, bool atomicKings, bool checksEnabled);

	void addMoveSquare(sf::Vector2i square);
	void addCaptureSquare(sf::Vector2i square);

};