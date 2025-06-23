#include "Chess.h"
#include <iostream>

std::string Chess::toString(Variant variant)
{
	switch (variant) {
	case Chess::Antichess:
		return "antichess";
	case Chess::Atomic:
		return "atomic";
	case Chess::Horde:
		return "horde";
	case Chess::RacingKings:
		return "racingkings";
	case Chess::KOTH:
		return "kingofthehill";
	case Chess::Crazyhouse:
		return "crazyhouse";
	case Chess::ThreeCheck:
		return "3check";
	case Chess::FiveCheck:
		return "5check";
	}
	return "chess";
}

sf::Vector2i Chess::convertChessNotationtoPosition(std::string notation)
{
	int x = (notation.at(0) - 'a') + 1;
	int y = (notation.at(1) - '0');
	return { x, y };
}

std::string Chess::convertPositiontoNotation(sf::Vector2i position)
{
	std::string notation;
	int x = position.x;
	int y = position.y;
	notation += (x + 'a') - 1;
	notation += (y + '0');
	return notation;
}

int Chess::convertXtoChar(int x)
{
	return convertPositiontoNotation({ x + 1, 0 }).front();
}

int Chess::convertChartoX(char c)
{
	return c - 'a';
}

sf::Vector2i Chess::reversePositon(sf::Vector2i position)
{
	return { reverseY(position.x), reverseY(position.y) };
}

sf::Vector2f Chess::reversePosition(sf::Vector2f position, sf::Vector2f boardSize)
{
	return (boardSize - position);
}

sf::Vector2f Chess::getGlobalPosition(sf::Vector2i position, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, bool reversed)
{
	sf::Vector2f r{ (((float)position.x - 0.5f) * boardMultiplier), (reverseY((float)position.y) - 0.5f) * boardMultiplier };
	if (reversed) { return Chess::reversePosition(r, boardSize) + boardOffset; }
	return r + boardOffset;
}

sf::Vector2f Chess::getGlobalPosition(sf::Vector2f position, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, bool reversed)
{
	sf::Vector2f r{ ((position.x - 0.5f) * boardMultiplier), (reverseY(position.y) - 0.5f) * boardMultiplier };
	if (reversed) { return Chess::reversePosition(r, boardSize) + boardOffset; }
	return r + boardOffset;
}

sf::Vector2i Chess::getLocalPosition(sf::Vector2f position, sf::Vector2f boardOffset, float boardMultiplier, bool reversed)
{
	sf::Vector2f v = { std::ceil((position.x - boardOffset.x) / boardMultiplier), reverseY(std::ceil((position.y - boardOffset.y) / boardMultiplier)) };
	sf::Vector2i d = (sf::Vector2i)v;
	if (reversed) { return { reverseY(d.x), reverseY(d.y) }; }
	return d;
}
