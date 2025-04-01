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
	int x, y, index;
	int pointValue;
	sf::Vector2i position;
	PColor color;
	sf::Texture texture;
	sf::Sprite sprite;
	std::string whiteIdentifier, blackIdentifier;
	std::vector<sf::Vector2i> availablePositions, availableCapturePositions;
	Piece(int x, int y, float scale, float xOffset, float boardMultiplier, int index, PColor color, sf::Texture& texture);
	virtual ~Piece();

	void move(int newX, int newY);
	void draw(sf::RenderWindow& window) { window.draw(sprite); }
	bool isWhite() const { return color == White; }
	bool isBlack() const { return color == Black; }
	std::string getIdentifier() const;
	std::vector<sf::Vector2i> getAvailablePositions() { return availablePositions; }
	std::vector<sf::Vector2i> getAvailableCapturePositions() { return availableCapturePositions; }
	bool operator==(const Piece& other) const { return pointValue == other.pointValue; }
	bool operator<(const Piece& other) const { return pointValue < other.pointValue; }
	bool operator>(const Piece& other) const { return pointValue > other.pointValue; }
	bool operator<=(const Piece& other) const { return pointValue <= other.pointValue; }
	bool operator>=(const Piece& other) const { return pointValue >= other.pointValue; }
	bool operator+(const Piece& other) const { return pointValue + other.pointValue; }
	bool operator-(const Piece& other) const { return pointValue - other.pointValue; }
	int reverseY(int y) { return 9 - y; }
	void setPosition(int x, int y) { this->x = x; this->y = y; }
	virtual void a() = 0;

private:
	float boardMultiplier, scale, xOffset;

};