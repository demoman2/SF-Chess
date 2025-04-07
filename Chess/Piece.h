#pragma once
#include <SFML/Main.hpp>
#include <SFML/Graphics.hpp>

class Piece
{
public:
	enum PColor {
		White,
		Black
	};
	std::string name;
	bool moving, captured;
	int x, y;
	int pointValue;
	sf::Vector2i position;
	std::optional<sf::Vector2f> targetPos = {};
	PColor color;
	size_t index;
	std::string whiteIdentifier, blackIdentifier;
	std::vector<sf::Vector2i> availablePositions, availableCapturePositions;
	std::vector<std::vector<sf::Vector2i>> allPositions;
	std::vector<sf::Sprite> selectionSquares, captureSquares;
	std::vector<std::vector<sf::Sprite>> allSquares;
	Piece(int x, int y, float scale, float boardOffset, float boardMultiplier, size_t index, PColor color, sf::Texture& texture);
	virtual ~Piece();
	virtual std::shared_ptr<Piece> clone() const = 0;


	void draw(sf::RenderWindow& window) { window.draw(sprite); }
	void drawGhostSprite(sf::RenderWindow& window) { window.draw(ghostSprite); }
	bool isWhite() const { return color == White; }
	bool isBlack() const { return color == Black; }
	std::string getIdentifier() const;
	std::vector<sf::Vector2i> getAvailablePositions() { return availablePositions; }
	std::vector<sf::Vector2i> getAvailableCapturePositions() { return availableCapturePositions; }
	float getBoardMultiplier() const { return boardMultiplier; }
	float getBoardOffset() const { return boardOffset; }
	float getScale() const { return scale; }
	bool operator==(const Piece& other) const { return pointValue == other.pointValue; }
	bool operator<(const Piece& other) const { return pointValue < other.pointValue; }
	bool operator>(const Piece& other) const { return pointValue > other.pointValue; }
	bool operator<=(const Piece& other) const { return pointValue <= other.pointValue; }
	bool operator>=(const Piece& other) const { return pointValue >= other.pointValue; }
	bool contains(sf::Vector2i pos) { return sprite.getGlobalBounds().contains((sf::Vector2f)pos); }
	int reverseY(int y) { return 9 - y; }
	void setGhostSpriteVisible(bool visible, bool pieceVisible);
	void setLocalPosition(sf::Vector2i pos);
	void setGlobalPosition(sf::Vector2f pos, bool ghost);
	void addCaptureSquare(sf::Vector2i square) { availableCapturePositions.push_back(square); }
	sf::Vector2i getLocalPosition() { return position; };
	sf::Vector2f getGlobalPosition() { return sprite.getPosition(); };
	void setTarget(std::optional<sf::Vector2f> target) {
		targetPos = target;
	};

private:
	float boardMultiplier, scale, boardOffset;
	sf::Texture texture;
	sf::Sprite sprite, ghostSprite;
	sf::Color ghostSpriteColor = sf::Color(255, 255, 255, 76);

};