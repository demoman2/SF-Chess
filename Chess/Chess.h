#pragma once
#include <string>
#include <vector>
#include <cmath>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace Chess
{

	enum PColor {
		White,
		Black
	};
	enum Endgame {
		None,
		Stalemate,
		Checkmate,
		InsufficientMaterial,
		FiftyMoveRule,
		ThreefoldRepetition,
		VariantVictory,
		VariantLoss
	};
	enum Variant {
		Standard,
		Atomic,
		Antichess,
		Horde,
		ThreeCheck,
		FiveCheck,
		RacingKings,
		KOTH,
		Crazyhouse
	};
	static PColor operator!(PColor c) {
		return static_cast<PColor>(c + 1 % 2);
	}

	std::string toString(Variant variant);

	sf::Vector2i convertChessNotationtoPosition(std::string notation);
	std::string convertPositiontoNotation(sf::Vector2i position);

	int convertXtoChar(int x);
	int convertChartoX(char c);

	template<typename T> static T reverseY(T y) { return 9 - y; }
	sf::Vector2i reversePositon(sf::Vector2i position);
	sf::Vector2f reversePosition(sf::Vector2f position, sf::Vector2f boardOffset);

	sf::Vector2f getGlobalPosition(sf::Vector2i position, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, bool reversed = false);
	sf::Vector2f getGlobalPosition(sf::Vector2f position, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, bool reversed = false);
	sf::Vector2i getLocalPosition(sf::Vector2f position, sf::Vector2f boardOffset, float boardMultiplier, bool reversed = false);
}
