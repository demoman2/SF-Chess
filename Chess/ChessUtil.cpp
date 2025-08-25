#include "ChessUtil.h"
#include "Piece.h"
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
	sf::Vector2f r{ ((float(position.x) - 0.5f) * boardMultiplier), (reverseY(float(position.y)) - 0.5f) * boardMultiplier };
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

bool Chess::isPieceAt(sf::Vector2i position, const pieceVector& pieces)
{
	return getPieceFromPosition(position, pieces) != nullptr;
}

std::shared_ptr<Piece> Chess::getPieceFromPosition(sf::Vector2i position, const pieceVector& pieces)
{
	for (auto& piece : pieces) {
		if (piece->getLocalPos() == position) {
			return piece;
		}
	}
	return nullptr;
}

pieceVector Chess::makePieceVec(const pieceVector& pieceList, sf::Vector2i square, sf::Vector2i localPos)
{
	pieceVector newV;
	newV.resize(pieceList.size());
	for (size_t j = 0; j < pieceList.size(); j++)
	{
		newV[j] = pieceList[j]->clone(false, false);
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

pieceVector Chess::makePieceVec(const pieceVector& pieceList, sf::Vector2i square, sf::Vector2i localPos, const Chess::Variant variant)
{
	bool isCapture = false;
	pieceVector newV;
	for (const auto& piece : pieceList)
	{
		newV.push_back(piece->clone(false, false));
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
						if (p && !p->hasID('p')) {
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

pieceVector Chess::copyPieceVec(const pieceVector& pieceList, bool copySprites, bool copySquares)
{
	pieceVector newV;
	for (auto& piece : pieceList) {
		newV.push_back(piece->clone(copySprites, copySquares));
	}
	return newV;
}

bool Chess::isValidSquare(sf::Vector2i square) {
	return (square.x <= 8 && square.x >= 1 && square.y <= 8 && square.y >= 1);
}

bool Chess::isValidPosition(const pieceVector& position, Chess::PColor color) {
	for (auto& piece : position) {
		if (piece->color != color && !piece->validatePosition(position)) {
			return false;
		}
	}
	return true;
}

bool Chess::isValidAtomicCapture(sf::Vector2i square, const pieceVector& pieces, Chess::PColor color) {
	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			sf::Vector2i newPos = { square.x + x, square.y + y };
			if (Chess::isValidSquare(newPos)) {
				if (Chess::getPieceFromPosition(newPos, pieces)) {
					std::shared_ptr<Piece> p = Chess::getPieceFromPosition(newPos, pieces);
					if (p->hasID('k') && p->color == color) {
						return false;
					}
				}
			}
		}
	}
	return true;
}

bool Chess::canCaptureEnemyKing(sf::Vector2i square, const pieceVector& pieces, Chess::PColor color) {
	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			sf::Vector2i newPos = { square.x + x, square.y + y };
			if (Chess::isValidSquare(newPos)) {
				if (Chess::getPieceFromPosition(newPos, pieces)) {
					std::shared_ptr<Piece> p = Chess::getPieceFromPosition(newPos, pieces);
					if (p->hasID('k') && p->color != color) {
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool Chess::enemyKingOccupies(sf::Vector2i square, const pieceVector& pieces, Chess::PColor color) {
	for (auto& piece : pieces) {
		if (piece->hasID('k') && piece->color != color) {
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

sf::Vector2f Interpolate(const sf::Vector2f pointA, const sf::Vector2f pointB, float factor) {
	if (std::fmax(std::abs((pointB - pointA).x), std::abs((pointB - pointA).y)) < 1.0f) {
		return pointB;
	}
	return (pointA * (1 - factor) + pointB * factor);
}
sf::Vector2f Interpolate(const sf::Vector2f pointA, const sf::Vector2f pointB, float factor, float threshold) {
	factor = std::clamp(factor, 0.0f, 1.0f);
	if (std::fmax(std::abs((pointB - pointA).x), std::abs((pointB - pointA).y)) < threshold) {
		return pointB;
	}
	return (pointA * (1 - factor) + pointB * factor);
}

std::string Chess::getNewFEN(Chess::Variant variant, bool chess960Enabled)
{
	std::string nFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w AHah - 0 1";
	if (chess960Enabled) {
		int id = 0;
		std::string fen = Chess::getRandomLineFrom("assets/fen/chess960.txt", id);
		if (!fen.empty()) {
			if (variant == Chess::ThreeCheck) {
				nFEN = fen.substr(0, fen.size() - 3);
				nFEN += "3+3 0 1";
			}
			else if (variant == Chess::FiveCheck) {
				nFEN = fen.substr(0, fen.size() - 3);
				nFEN += "5+5 0 1";
			}
			else if (variant == Chess::Horde) {
				nFEN = fen.substr(0, 8) + "/pppppppp/8/1PP2PP1/PPPPPPPP/PPPPPPPP/PPPPPPPP/PPPPPPPP w kq - 0 1";
			}
			else if (variant == Chess::RacingKings) {
				nFEN = "8/8/8/8/8/8/krbnNBRK/qrbnNBRQ w - - 0 1";
			}
			else if (variant == Chess::Crazyhouse) {
				nFEN = fen;
				nFEN.insert(43, "[]");
			}
			else {
				nFEN = fen;
			}
		}
		else {
			std::cout << "Empty Chess960 string returnd!" << std::endl;
		}
	}
	else if (variant == Chess::Horde) {
		nFEN = "rnbqkbnr/pppppppp/8/1PP2PP1/PPPPPPPP/PPPPPPPP/PPPPPPPP/PPPPPPPP w kq - 0 1";
	}
	else if (variant == Chess::ThreeCheck) {
		nFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 3+3 0 1";
	}
	else if (variant == Chess::FiveCheck) {
		nFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 5+5 0 1";
	}
	else if (variant == Chess::RacingKings) {
		nFEN = "8/8/8/8/8/8/krbnNBRK/qrbnNBRQ w - - 0 1";
	}
	else if (variant == Chess::Crazyhouse) {
		nFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR[] w KQkq - 0 1";
	}
	return nFEN;
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

std::string Chess::getRandomLineFrom(std::string filePath)
{
	std::ifstream readFile(filePath);
	std::string line;
	int lines = 0;
	while (std::getline(readFile, line)) { lines++; }
	int id = (std::rand() % lines) + 1;
	readFile.clear();
	readFile.seekg(0, readFile.beg);
	line.clear();
	for (int i = 1; i <= id; i++) {
		std::getline(readFile, line);
	}
	readFile.close();
	return line;
}

std::string Chess::getRandomLineFrom(std::string filePath, int& id)
{
	std::ifstream readFile(filePath);
	std::string line;
	int lines = 0;
	while (std::getline(readFile, line)) { lines++; }
	id = (std::rand() % lines) + 1;
	readFile.clear();
	readFile.seekg(0, readFile.beg);
	line.clear();
	for (int i = 1; i <= id; i++) {
		std::getline(readFile, line);
	}
	readFile.close();
	return line;
}

bool randomBool() {
	static auto gen = std::bind(std::uniform_int_distribution<>(0, 1), std::default_random_engine());
	return gen();
}