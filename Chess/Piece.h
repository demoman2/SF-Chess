#pragma once
#include <SFML/Main.hpp>
#include <SFML/Graphics.hpp>
#include "PieceColor.cpp"

class Piece
{
public:
	enum PColor {
		White,
		Black
	};
	std::string name;
	float boardMultiplier, scale, xOffset;
	int x, y;
	int pointValue;
	sf::Vector2i position;
	PColor color;
	sf::Texture texture;
	sf::Sprite sprite;
	std::string whiteIdentifier, blackIdentifier;
	std::vector<sf::Vector2i> availablePositions, availableCapturePositions;
	std::vector<std::unique_ptr<Piece>>& pieces;
	// std::unique_ptr<Piece> instead of Piece* 
	Piece(int x, int y, float scale, float xOffset, float boardMultiplier, PColor color, sf::Texture& texture, std::vector<std::unique_ptr<Piece>>& pieces);
	~Piece();

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
	virtual void calculatePositions();
	std::optional<Piece> getPieceFromPosition(sf::Vector2i position);

private:
	std::vector<Piece>::iterator it;

};