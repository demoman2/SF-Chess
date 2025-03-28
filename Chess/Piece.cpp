#include "Piece.h"

Piece::Piece(int x, int y, float scale, float xOffset, float boardMultiplier, PColor color, sf::Texture& texture, std::vector<std::unique_ptr<Piece>>& pieces)
	: sprite(texture), position(x, y), pieces(pieces)
{
	sprite.setPosition(sf::Vector2f{ xOffset + (x * boardMultiplier), y * boardMultiplier });
	sprite.setScale(sf::Vector2f(scale, scale));

}

Piece::~Piece()
{

}

void Piece::move(int newX, int newY)
{
	x = newX;
	y = newY;
	sprite.setPosition(sf::Vector2f{ newX * boardMultiplier, newY * boardMultiplier });
	calculatePositions();
}

std::string Piece::getIdentifier() const
{
	if (isWhite()) {
		return whiteIdentifier;
	}
	return blackIdentifier;
}

std::optional<Piece> Piece::getPieceFromPosition(sf::Vector2i position)
{
	for (auto& piece : pieces) {
		if (piece->position == position) {
			return *piece;
		}
	}
	return {};
}


void Piece::calculatePositions()
{

}
