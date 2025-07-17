#include "Pawn.h"

Pawn::Pawn(int x, int y, float scale, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, Chess::PColor color, sf::Texture& texture, bool animated, bool promoted, bool reversed)
	: Piece(x, y, scale, boardOffset, boardSize, boardMultiplier, color, texture, animated, promoted, reversed)
{
	positionVectors.push_back(&enPassantPositions);
	id = 'p';
	enPassantTarget = false;
}

Pawn::Pawn(bool enPassantTarget, int x, int y, float scale, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, Chess::PColor color, sf::Texture& texture, bool animated, bool promoted, bool reversed) :
	Piece(x, y, scale, boardOffset, boardSize, boardMultiplier, color, texture, animated, promoted, reversed)
{
	positionVectors.push_back(&enPassantPositions);
	id = 'p';
	this->enPassantTarget = enPassantTarget;
}

Pawn::Pawn(const Pawn& other, bool copySprite, bool copySquares) : Piece(other, copySprite, copySquares), enPassantTarget(other.enPassantTarget)
{
	if (copySquares) {
		enPassantPositions = other.enPassantPositions;
	}
	positionVectors.push_back(&enPassantPositions);
}

Pawn::~Pawn()
{
}

std::shared_ptr<Piece> Pawn::clone(bool copySprite, bool copySquares) const
{
	return std::make_shared<Pawn>(*this, copySprite, copySquares);
}

bool Pawn::isValidPassant(sf::Vector2i square, const pieceVector& pieceList, Chess::Variant variant, bool atomicKings, bool checksEnabled)
{
	if (checksEnabled) {
		if (!atomicKings) {
			pieceVector newV{};
			newV.resize(pieceList.size());
			for (size_t j = 0; j < pieceList.size(); ++j)
			{
				newV[j] = pieceList[j]->clone(false, false);
			}
			sf::Vector2i enPassantPos{ 0, 0 };
			if (color == Chess::PColor::White) {
				enPassantPos = { square.x, square.y - 1 };
			}
			else {
				enPassantPos = { square.x, square.y + 1 };
			}
			for (int j = 0; j < newV.size(); j++) {
				if (newV.at(j)->getLocalPos() == enPassantPos) {
					newV.erase(newV.begin() + j);
					break;
				}
			}
			for (auto& v : newV) {
				if (v->getLocalPos() == getLocalPos()) {
					v->setLocalPosition(square);
				}
			}
			if (variant == Chess::Atomic) {
				std::vector<sf::Vector2i> piecePositions{ square };
				for (int x = -1; x <= 1; x++) {
					for (int y = -1; y <= 1; y++) {
						sf::Vector2i newPos = { square.x + x, square.y + y };
						if (Chess::isValidSquare(newPos)) {
							std::shared_ptr<Piece> p = Chess::getPieceFromPosition(newPos, newV);
							if (p != nullptr && !p->hasID('p')) {
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
				if (!isValidPosition(newV, color)) {
					if (!(Chess::canCaptureEnemyKing(square, pieceList, color) && Chess::isValidAtomicCapture(square, pieceList, color))) {
						return false;
					}
				}
				else if (!Chess::isValidAtomicCapture(square, pieceList, color)) {
					return false;
				}
			}
			else {
				if (!Chess::isValidPosition(newV, color)) {
					return false;
				}
			}
		}
		else {
			if (!Chess::isValidAtomicCapture(square, pieceList, color)) {
				return false;
			}
		}
	}
	return true;
}

void Pawn::generateLegalMoves(const pieceVector& pieceList, Chess::Variant variant, bool atomicKings, bool checksEnabled, bool castlingEnabled, bool chess960)
{
	canMove = false;
	availablePositions.clear();
	availableCapturePositions.clear();
	enPassantPositions.clear();
	sf::Vector2i pos, pos2, pos3, pos4;
	if (color == Chess::PColor::White) {
		pos = { position.x, position.y + 1 }, pos2 = { position.x, position.y + 2 };
		pos3 = { position.x - 1, position.y + 1 }, pos4 = { position.x + 1, position.y + 1 };
	}
	else {
		pos = { position.x, position.y - 1 }, pos2 = { position.x, position.y - 2 };
		pos3 = { position.x - 1, position.y - 1 }, pos4 = { position.x + 1, position.y - 1 };
	}
	if (Chess::isValidSquare(pos) && Chess::getPieceFromPosition(pos, pieceList) == nullptr) {
		if (isValidMove(pos, pieceList, variant, atomicKings, checksEnabled)) {
			addMoveSquare(pos);
		}
		if (((isWhite() && position.y == 2) || (isBlack() && position.y == 7)) || (variant == Chess::Horde && isWhite() && position.y == 1)) {
			if (Chess::isValidSquare(pos2) && Chess::getPieceFromPosition(pos2, pieceList) == nullptr) {
				if (isValidMove(pos2, pieceList, variant, atomicKings, checksEnabled)) {
					addMoveSquare(pos2);
				}
			}
		}
	}
	if (Chess::isValidSquare(pos3)) {
		std::shared_ptr<Piece> p = Chess::getPieceFromPosition(pos3, pieceList);
		if (p != nullptr && p->color != color) {
			if (isValidCapture(pos3, pieceList, variant, atomicKings, checksEnabled)) {
				addCaptureSquare(pos3);
				p->addAttacker();
			}
		}
	}
	if (Chess::isValidSquare(pos4)) {
		std::shared_ptr<Piece> p = Chess::getPieceFromPosition(pos4, pieceList);
		if (p != nullptr && p->color != color) {
			if (isValidCapture(pos4, pieceList, variant, atomicKings, checksEnabled)) {
				addCaptureSquare(pos4);
				p->addAttacker();
			}
		}
	}
	// En Passant
	std::shared_ptr<Piece> temp = Chess::getPieceFromPosition(sf::Vector2i{ position.x + 1, position.y }, pieceList);
	if (temp != nullptr && temp->color != color && temp->hasID('p')) {
		std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(temp);
		if (pawn->enPassantTarget) {
			if (pawn->color == Chess::PColor::White) {
				if (isValidPassant({ pawn->getLocalPos().x, pawn->getLocalPos().y - 1 }, pieceList, variant, atomicKings, checksEnabled)) {
					addEnPassantSquare(sf::Vector2i{ pawn->getLocalPos().x, pawn->getLocalPos().y - 1 });
				}
			}
			else {
				if (isValidPassant({ pawn->getLocalPos().x, pawn->getLocalPos().y + 1 }, pieceList, variant, atomicKings, checksEnabled)) {
					addEnPassantSquare(sf::Vector2i{ pawn->getLocalPos().x, pawn->getLocalPos().y + 1 });
				}
			}
		}
	}
	temp = Chess::getPieceFromPosition(sf::Vector2i{ position.x - 1, position.y }, pieceList);
	if (temp != nullptr && temp->color != color && temp->hasID('p')) {
		std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(temp);
		if (pawn->enPassantTarget) {
			if (pawn->color == Chess::PColor::White) {
				if (isValidPassant({ pawn->getLocalPos().x, pawn->getLocalPos().y - 1 }, pieceList, variant, atomicKings, checksEnabled)) {
					addEnPassantSquare(sf::Vector2i{ pawn->getLocalPos().x, pawn->getLocalPos().y - 1 });
				}
			}
			else {
				if (isValidPassant({ pawn->getLocalPos().x, pawn->getLocalPos().y + 1 }, pieceList, variant, atomicKings, checksEnabled)) {
					addEnPassantSquare(sf::Vector2i{ pawn->getLocalPos().x, pawn->getLocalPos().y + 1 });
				}
			}
		}
	}

	if (!availablePositions.empty()) {
		canMove = true;
	}
	if (!availableCapturePositions.empty()) {
		canMove = true;
	}
	if (!enPassantPositions.empty()) {
		canMove = true;
	}
}

bool Pawn::validatePosition(const pieceVector& pieceList)
{
	sf::Vector2i pos, pos2;
	if (color == Chess::PColor::White) {
		pos = { position.x - 1,position.y + 1 }, pos2 = { position.x + 1,position.y + 1 };
	}
	else {
		pos = { position.x - 1, position.y - 1 }, pos2 = { position.x + 1, position.y - 1 };
	}
	if (Chess::isValidSquare(pos)) {
		if (Chess::getPieceFromPosition(pos, pieceList) != nullptr) {
			std::shared_ptr<Piece> p = Chess::getPieceFromPosition(pos, pieceList);
			if (p->hasID('k') && p->color != color) {
				return false;
			}
		}
	}
	if (Chess::isValidSquare(pos2)) {
		if (Chess::getPieceFromPosition(pos2, pieceList) != nullptr) {
			std::shared_ptr<Piece> p = Chess::getPieceFromPosition(pos2, pieceList);
			if (p->hasID('k') && p->color != color) {
				return false;
			}
		}
	}
	return true;
}

void Pawn::updateSprites(std::vector<sf::Texture>& boardTextures, sf::Vector2f mousePos, bool& mouseSelecting)
{
	for (auto& square : availablePositions) {
		if (square.sprite != nullptr) {
			auto& sprite = square.sprite;
			if (sprite->getGlobalBounds().contains(sf::Vector2f(mousePos))) {
				mouseSelecting = true;
				sprite->setTexture(boardTextures.at(4), true);
			}
			else { sprite->setTexture(boardTextures.at(0), true); }
		}
	}
	for (auto& square : availableCapturePositions) {
		if (square.sprite != nullptr) {
			auto& sprite = square.sprite;
			if (sprite->getGlobalBounds().contains(sf::Vector2f(mousePos))) {
				mouseSelecting = true;
				sprite->setTexture(boardTextures.at(4));
			}
			else { sprite->setTexture(boardTextures.at(1)); }
		}
	}
	for (auto& square : enPassantPositions) {
		auto& sprite = square.sprite;
		if (sprite->getGlobalBounds().contains(sf::Vector2f(mousePos))) {
			mouseSelecting = true;
			sprite->setTexture(boardTextures.at(4));
		}
		else { sprite->setTexture(boardTextures.at(0)); }
	}
}

void Pawn::addEnPassantSquare(sf::Vector2i square)
{
	std::string m = Chess::convertPositiontoNotation(position) + Chess::convertPositiontoNotation(square);
	enPassantPositions.emplace_back(square, m);
}
