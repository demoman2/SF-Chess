#include "ChessUtil.h"
#include "Piece.h"

// Drop Piece
Chess::DropPiece::DropPiece(char id, sf::Texture& texture, sf::Texture& backgroundTexture, sf::Texture& textBackgroundTexture, sf::Font& font, sf::Vector2f dropPiecePosition, float dropPieceSquareSize, float bgStart, size_t i)
	: id(id), count(0), sprite(texture), background(backgroundTexture), textBG(textBackgroundTexture), text(font, std::to_string(count), 32)
{
	sprite.setPosition({ dropPiecePosition.x, bgStart + dropPieceSquareSize * i });
	sprite.setOrigin(sprite.getLocalBounds().getCenter());
	sprite.setScale({ dropPieceSquareSize / texture.getSize().x, dropPieceSquareSize / texture.getSize().y });
	background.setOrigin(background.getLocalBounds().getCenter());
	background.setScale(sprite.getScale());
	background.setPosition(sprite.getPosition());
	background.setColor({ 255, 255, 255, 0 });

	textBG.setOrigin(textBG.getLocalBounds().getCenter());
	textBG.setScale(sprite.getScale());
	textBG.setPosition(sprite.getPosition());

	text.setScale({ sprite.getScale().x * 3, sprite.getScale().y * 3 });
	text.setOrigin(text.getLocalBounds().getCenter());
	text.setPosition(sprite.getPosition() + sf::Vector2f{ dropPieceSquareSize * 0.35f, dropPieceSquareSize * 0.32f });

}

void Chess::DropPiece::update()
{
	text.setString(std::to_string(count));
	if (count == 0) {
		sprite.setColor(sf::Color(sprite.getColor().r, sprite.getColor().g, sprite.getColor().b, 26));
		textBG.setColor(sf::Color(textBG.getColor().r, textBG.getColor().g, textBG.getColor().b, 0));
		text.setFillColor(sf::Color(text.getFillColor().r, text.getFillColor().g, text.getFillColor().b, 0));
	}
	else {
		sprite.setColor(sf::Color(sprite.getColor().r, sprite.getColor().g, sprite.getColor().b, 255));
		textBG.setColor(sf::Color(textBG.getColor().r, textBG.getColor().g, textBG.getColor().b, 255));
		text.setFillColor(sf::Color(text.getFillColor().r, text.getFillColor().g, text.getFillColor().b, 255));
	}
}

void Chess::DropPiece::move(float x, float y) {
	sprite.move({ x, y });
	textBG.move({ x, y });
	text.move({ x, y });
	background.move({ x, y });
}

void Chess::DropPiece::scale(float dropPieceSquareSize, sf::Sprite& dropPieceBackground, float bgStart, int i)
{
	sprite.setScale({ dropPieceSquareSize / sprite.getTexture().getSize().x, dropPieceSquareSize / sprite.getTexture().getSize().y});
	textBG.setScale(sprite.getScale());
	text.setScale({ sprite.getScale().x * 3, sprite.getScale().y * 3 });
	background.setScale(sprite.getScale());
	sprite.setPosition({ dropPieceBackground.getPosition().x, bgStart + dropPieceSquareSize * i });
	textBG.setPosition(sprite.getPosition());
	text.setPosition(sprite.getPosition() + sf::Vector2f{ dropPieceSquareSize * 0.35f, dropPieceSquareSize * 0.32f });
	background.setPosition(sprite.getPosition());
}

bool Chess::DropPiece::mouseSelecting(sf::Vector2f mousePos) const
{
	return count != 0 && sprite.getGlobalBounds().contains(mousePos);
}

void Chess::DropPiece::draw(sf::RenderWindow& window) const
{
	window.draw(background);
	window.draw(sprite);
	window.draw(textBG);
	window.draw(text);
}

// Promote Piece
Chess::PromotePiece::PromotePiece(sf::Texture& pieceTexture, sf::Texture& backgroundTexture, sf::Vector2i pos, char id, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, float pieceScale, sf::Vector2f boardScale, sf::Color promotionSquareColor, sf::Vector2f scale) :
	sprite(pieceTexture), backgroundSprite(backgroundTexture), id(id), initialScale(scale)
{
	backgroundSprite.setOrigin(backgroundSprite.getGlobalBounds().getCenter());
	backgroundSprite.setScale(boardScale);
	backgroundSprite.setPosition(Chess::getGlobalPosition(pos, boardOffset, boardSize, boardMultiplier));
	backgroundSprite.setColor(promotionSquareColor);
	sprite.setPosition(Chess::getGlobalPosition(pos, boardOffset, boardSize, boardMultiplier));
	sprite.setOrigin(sprite.getLocalBounds().getCenter());
	sprite.setScale(scale);
}

// Misc
std::vector<std::string> split(std::string str, char del) {
	std::vector<std::string> res;
	std::string temp;
	for (int i = 0; i < str.size(); i++) {
		if (str.at(i) == del) {
			res.push_back(temp);
			temp = "";
		}
		else {
			temp += str.at(i);
		}
	}
	res.push_back(temp);
	return res;
}

size_t findNthOf(std::string str, char find, int nth)
{
	int occ = 0;
	for (size_t i = 0; i < str.size(); i++) {
		if (str.at(i) == find) { occ++; }
		if (occ == nth) { return i; }
	}
	return 0;
}

sf::Vector2f Interpolate(const sf::Vector2f pointA, const sf::Vector2f pointB, float factor) {
	if (std::fmax(std::abs((pointB - pointA).x), std::abs((pointB - pointA).y)) < 1.0f) {
		return pointB;
	}
	return pointA + (pointB - pointA) * std::clamp(factor, 0.0f, 1.0f);
}
sf::Vector2f Interpolate(const sf::Vector2f pointA, const sf::Vector2f pointB, float factor, float threshold) {
	if (std::fmax(std::abs((pointB - pointA).x), std::abs((pointB - pointA).y)) < threshold) {
		return pointB;
	}
	return pointA + (pointB - pointA) * std::clamp(factor, 0.0f, 1.0f);
}

uint8_t Interpolate(uint8_t current, uint8_t target, float factor)
{
	return (uint8_t)current + (target - current) * factor;
}

void setSpriteVisible(sf::Sprite& sprite, bool visible)
{
	if (visible) {
		sprite.setColor({ sprite.getColor().r, sprite.getColor().g, sprite.getColor().b, 255 });
	}
	else {
		sprite.setColor({ sprite.getColor().r, sprite.getColor().g, sprite.getColor().b, 0 });
	}
}

void setSpriteVisible(sf::Sprite& sprite, bool visible, uint8_t vAlpha)
{
	if (visible) {
		sprite.setColor({ sprite.getColor().r, sprite.getColor().g, sprite.getColor().b, vAlpha });
	}
	else {
		sprite.setColor({ sprite.getColor().r, sprite.getColor().g, sprite.getColor().b, 0 });
	}
}

// Bishop, King, Knight, Pawn, Queen, Rook
std::vector<sf::Texture> Chess::makePieceSet(sf::Image spriteSheet, int pieceCount, int pieceSize) {
	std::vector<sf::Texture> pieceTextures;
	for (size_t i = 0; i < pieceCount; i++) {
		switch (i) {
			// Black
		case 0:
			pieceTextures.emplace_back(spriteSheet, false, sf::IntRect({ 0, 0 }, { pieceSize, pieceSize }));
			break;
		case 1:
			pieceTextures.emplace_back(spriteSheet, false, sf::IntRect({ pieceSize, 0 }, { pieceSize, pieceSize }));
			break;
		case 2:
			pieceTextures.emplace_back(spriteSheet, false, sf::IntRect({ pieceSize * 2, 0 }, { pieceSize, pieceSize }));
			break;
		case 3:
			pieceTextures.emplace_back(spriteSheet, false, sf::IntRect({ 0, pieceSize }, { pieceSize, pieceSize }));
			break;
		case 4:
			pieceTextures.emplace_back(spriteSheet, false, sf::IntRect({ pieceSize, pieceSize }, { pieceSize, pieceSize }));
			break;
		case 5:
			pieceTextures.emplace_back(spriteSheet, false, sf::IntRect({ pieceSize * 2, pieceSize }, { pieceSize, pieceSize }));
			break;
			// White
		case 6:
			pieceTextures.emplace_back(spriteSheet, false, sf::IntRect({ 0, pieceSize * 2 }, { pieceSize, pieceSize }));
			break;
		case 7:
			pieceTextures.emplace_back(spriteSheet, false, sf::IntRect({ pieceSize, pieceSize * 2 }, { pieceSize, pieceSize }));
			break;
		case 8:
			pieceTextures.emplace_back(spriteSheet, false, sf::IntRect({ pieceSize * 2, pieceSize * 2 }, { pieceSize, pieceSize }));
			break;
		case 9:
			pieceTextures.emplace_back(spriteSheet, false, sf::IntRect({ 0, pieceSize * 3 }, { pieceSize, pieceSize }));
			break;
		case 10:
			pieceTextures.emplace_back(spriteSheet, false, sf::IntRect({ pieceSize, pieceSize * 3 }, { pieceSize, pieceSize }));
			break;
		case 11:
			pieceTextures.emplace_back(spriteSheet, false, sf::IntRect({ pieceSize * 2, pieceSize * 3 }, { pieceSize, pieceSize }));
			break;
		default:
			pieceTextures.emplace_back(spriteSheet, false, sf::IntRect({ 0, 0 }, { pieceSize, pieceSize }));
			break;
		}
		pieceTextures.back().setSmooth(true);
	}
	return pieceTextures;
};
void Chess::loadPieceSet(sf::Image& spriteSheet, std::vector<sf::Texture>& pieceTextures, int pieceSize) {
	for (size_t i = 0; i < pieceTextures.size(); i++) {
		if (!pieceTextures.at(i).isSmooth()) {
			pieceTextures.at(i).setSmooth(true);
		}
		switch (i) {
			// Black
		case 0:
			pieceTextures.at(i).loadFromImage(spriteSheet, false, sf::IntRect({ 0, 0 }, { pieceSize, pieceSize }));
			break;
		case 1:
			pieceTextures.at(i).loadFromImage(spriteSheet, false, sf::IntRect({ pieceSize, 0 }, { pieceSize, pieceSize }));
			break;
		case 2:
			pieceTextures.at(i).loadFromImage(spriteSheet, false, sf::IntRect({ pieceSize * 2, 0 }, { pieceSize, pieceSize }));
			break;
		case 3:
			pieceTextures.at(i).loadFromImage(spriteSheet, false, sf::IntRect({ 0, pieceSize }, { pieceSize, pieceSize }));
			break;
		case 4:
			pieceTextures.at(i).loadFromImage(spriteSheet, false, sf::IntRect({ pieceSize, pieceSize }, { pieceSize, pieceSize }));
			break;
		case 5:
			pieceTextures.at(i).loadFromImage(spriteSheet, false, sf::IntRect({ pieceSize * 2, pieceSize }, { pieceSize, pieceSize }));
			break;
			// White
		case 6:
			pieceTextures.at(i).loadFromImage(spriteSheet, false, sf::IntRect({ 0, pieceSize * 2 }, { pieceSize, pieceSize }));
			break;
		case 7:
			pieceTextures.at(i).loadFromImage(spriteSheet, false, sf::IntRect({ pieceSize, pieceSize * 2 }, { pieceSize, pieceSize }));
			break;
		case 8:
			pieceTextures.at(i).loadFromImage(spriteSheet, false, sf::IntRect({ pieceSize * 2, pieceSize * 2 }, { pieceSize, pieceSize }));
			break;
		case 9:
			pieceTextures.at(i).loadFromImage(spriteSheet, false, sf::IntRect({ 0, pieceSize * 3 }, { pieceSize, pieceSize }));
			break;
		case 10:
			pieceTextures.at(i).loadFromImage(spriteSheet, false, sf::IntRect({ pieceSize, pieceSize * 3 }, { pieceSize, pieceSize }));
			break;
		case 11:
			pieceTextures.at(i).loadFromImage(spriteSheet, false, sf::IntRect({ pieceSize * 2, pieceSize * 3 }, { pieceSize, pieceSize }));
			break;
		default:
			pieceTextures.at(i).loadFromImage(spriteSheet, false, sf::IntRect({ 0, 0 }, { pieceSize, pieceSize }));
			break;
		}
	}
};
void Chess::loadBoard(sf::Image& spriteSheet, sf::Sprite& board, sf::Texture& boardTexture, int boardNumber, int boardSize) {
	int y = std::floor(boardNumber / 4.0f);
	int x = boardNumber % 4;
	boardTexture.loadFromImage(spriteSheet, false, sf::IntRect({ x * boardSize, y * boardSize }, { boardSize, boardSize }));
	board.setTexture(boardTexture);
}

bool Chess::isPieceAt(sf::Vector2i position, const std::vector<std::shared_ptr<Piece>>& pieces)
{
	return getPieceFromPosition(position, pieces) != nullptr;
}

std::shared_ptr<Piece> Chess::getPieceFromPosition(sf::Vector2i position, const std::vector<std::shared_ptr<Piece>>& pieces)
{
	for (auto& piece : pieces) {
		if (piece->getLocalPos() == position) {
			return piece;
		}
	}
	return nullptr;
}

std::vector<std::shared_ptr<Piece>> Chess::makePieceVec(const std::vector<std::shared_ptr<Piece>>& pieceList, sf::Vector2i square, sf::Vector2i localPos)
{
	std::vector<std::shared_ptr<Piece>> newV;
	newV.resize(pieceList.size());
	for (size_t j = 0; j < pieceList.size(); j++)
	{
		newV[j] = pieceList[j]->clone();
	}
	for (int j = 0; j < newV.size(); j++) {
		if (newV.at(j)->getLocalPos() == square) {
			newV.erase(newV.begin() + j);
			break;
		}
	}
	for (auto& v : newV) {
		if (v->getLocalPos() == localPos) {
			v->setLocalPosition(square);
		}
	}
	return newV;
}

std::vector<std::shared_ptr<Piece>> Chess::makePieceVec(const std::vector<std::shared_ptr<Piece>>& pieceList, sf::Vector2i square, sf::Vector2i localPos, const Chess::Variant variant)
{
	bool isCapture = false;
	std::vector<std::shared_ptr<Piece>> newV;
	newV.resize(pieceList.size());
	for (size_t j = 0; j < pieceList.size(); j++)
	{
		newV[j] = pieceList[j]->clone();
	}
	for (int j = 0; j < newV.size(); j++) {
		if (newV.at(j)->getLocalPos() == square) {
			newV.erase(newV.begin() + j);
			isCapture = true;
			break;
		}
	}
	for (auto& v : newV) {
		if (v->getLocalPos() == localPos) {
			v->setLocalPosition(square);
		}
	}
	if (isCapture) {
		if (variant == Chess::Atomic) {
			std::vector<sf::Vector2i> piecePositions{ square };
			for (int x = -1; x <= 1; x++) {
				for (int y = -1; y <= 1; y++) {
					sf::Vector2i newPos = { square.x + x, square.y + y };
					if (Chess::isValidSquare(newPos)) {
						std::shared_ptr<Piece> p = Chess::getPieceFromPosition(newPos, newV);
						if (p != nullptr && p->name != "Pawn") {
							piecePositions.push_back(newPos);
						}
					}
				}
			}
			for (auto& pos : piecePositions) {
				for (auto it2 = newV.begin(); it2 != newV.end(); it2++) {
					std::shared_ptr<Piece> p = *it2;
					if (p->getLocalPos() == pos) {
						newV.erase(it2);
						break;
					}
				}
			}
		}
	}

	return newV;
}

bool Chess::isValidSquare(sf::Vector2i square) {
	return (square.x <= 8 && square.x >= 1 && square.y <= 8 && square.y >= 1);
}

bool Chess::isValidPosition(const std::vector<std::shared_ptr<Piece>>& position, Chess::PColor color) {
	for (auto& piece : position) {
		if (piece->color != color && !piece->validatePosition(position)) {
			return false;
		}
	}
	return true;
}

bool Chess::isValidAtomicCapture(sf::Vector2i square, const std::vector<std::shared_ptr<Piece>>& pieces, Chess::PColor color) {
	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			sf::Vector2i newPos = { square.x + x, square.y + y };
			if (Chess::isValidSquare(newPos)) {
				if (Chess::getPieceFromPosition(newPos, pieces) != nullptr) {
					std::shared_ptr<Piece> p = Chess::getPieceFromPosition(newPos, pieces);
					if (p->IsA("King") && p->color == color) {
						return false;
					}
				}
			}
		}
	}
	return true;
}

bool Chess::canCaptureEnemyKing(sf::Vector2i square, const std::vector<std::shared_ptr<Piece>>& pieces, Chess::PColor color) {
	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			sf::Vector2i newPos = { square.x + x, square.y + y };
			if (Chess::isValidSquare(newPos)) {
				if (Chess::getPieceFromPosition(newPos, pieces) != nullptr) {
					std::shared_ptr<Piece> p = Chess::getPieceFromPosition(newPos, pieces);
					if (p->IsA("King") && p->color != color) {
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool Chess::enemyKingOccupies(sf::Vector2i square, const std::vector<std::shared_ptr<Piece>>& pieces, Chess::PColor color) {
	for (auto& piece : pieces) {
		if (piece->IsA("King") && piece->color != color) {
			for (int x = -1; x <= 1; x++) {
				for (int y = -1; y <= 1; y++) {
					if (x != 0 || y != 0) {
						sf::Vector2i newPos = { piece->getLocalPos().x + x, piece->getLocalPos().y + y };
						if (Chess::isValidSquare(newPos) && newPos == square) {
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

bool Chess::isInCenter(sf::Vector2i pos)
{
	return (pos.x == 4 || pos.x == 5) && (pos.y == 4 || pos.y == 5);
}

bool Chess::isValidDropSquare(char id, sf::Vector2i pos)
{
	return id != 'p' || (id == 'p' && pos.y != 1 && pos.y != 8);
}

Chess::SDropPiece::SDropPiece(char id, sf::Texture& texture) : id(id), sprite(texture)
{
}

void Chess::SDropPiece::set(sf::Texture& texture, int id)
{
	sprite.setTexture(texture);
	this->id = id;
}

// Square
Chess::Square::Square(sf::Sprite& sprite, sf::Vector2i position) : pos(position), moveString(""), sprite(sprite)
{
}

Chess::Square::Square(sf::Vector2i position, std::string moveString) : pos(position), moveString(moveString), sprite({})
{
}

void Chess::Square::setupSprite(sf::Texture& texture, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, float pieceScale, bool reversed) {
	float scale = (pieceScale * 320.0f) / 128.0f;
	sf::Sprite sSprite{ texture };
	sSprite.setOrigin(sSprite.getLocalBounds().getCenter());
	sSprite.setScale(sf::Vector2f(scale, scale));
	sSprite.setPosition(Chess::getGlobalPosition(pos, boardOffset, boardSize, boardMultiplier, reversed));
	sprite = sSprite;
}

void Chess::Square::setTexture(sf::Texture& texture) {
	if (sprite.has_value()) {
		sprite.value().setTexture(texture);
	}
}
