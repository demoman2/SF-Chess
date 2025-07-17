#include "Rook.h"

Rook::Rook(int x, int y, float scale, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, Chess::PColor color, sf::Texture& texture, bool animated, bool promoted, bool reversed)
	: Piece(x, y, scale, boardOffset, boardSize, boardMultiplier, color, texture, animated, promoted, reversed)
{
	id = 'r';
}

Rook::Rook(const Rook& other, bool copySprite, bool copySquares) : Piece(other, copySprite, copySquares)
{
}

Rook::~Rook()
{
}

std::shared_ptr<Piece> Rook::clone(bool copySprite, bool copySquares) const
{
	return std::make_shared<Rook>(*this, copySprite, copySquares);
}

void Rook::generateLegalMoves(const pieceVector& pieceList, Chess::Variant variant, bool atomicKings, bool checksEnabled, bool castlingEnabled, bool chess960)
{
	canMove = false;
	availablePositions.clear();
	availableCapturePositions.clear();
	// Up
	for (int i = 1; i < 8; i++) {
		int newX = position.x, newY = position.y + i;
		if (newY > 8) { break; }
		sf::Vector2i newPos = { newX, newY };
		if (Chess::isPieceAt(newPos, pieceList)) {
			std::shared_ptr<Piece> piece = Chess::getPieceFromPosition(newPos, pieceList);
			if (piece->color != color) {
				if (isValidCapture(newPos, pieceList, variant, atomicKings, checksEnabled)) {
					addCaptureSquare(newPos);
					piece->addAttacker();
				}
			}
			break;
		}
		else {
			if (isValidMove(newPos, pieceList, variant, atomicKings, checksEnabled)) {
				addMoveSquare(newPos);
			}
		}
	}
	// Down
	for (int i = 1; i < 8; i++) {
		int newX = position.x, newY = position.y - i;
		if (newY < 1) { break; }
		sf::Vector2i newPos = { newX, newY };
		if (Chess::isPieceAt(newPos, pieceList)) {
			std::shared_ptr<Piece> piece = Chess::getPieceFromPosition(newPos, pieceList);
			if (piece->color != color) {
				if (isValidCapture(newPos, pieceList, variant, atomicKings, checksEnabled)) {
					addCaptureSquare(newPos);
					piece->addAttacker();
				}
			}
			break;
		}
		else {
			if (isValidMove(newPos, pieceList, variant, atomicKings, checksEnabled)) {
				addMoveSquare(newPos);
			}
		}
	}
	// Right
	for (int i = 1; i < 8; i++) {
		int newX = position.x + i, newY = position.y;
		if (newX > 8) { break; }
		sf::Vector2i newPos = { newX, newY };
		if (Chess::isPieceAt(newPos, pieceList)) {
			std::shared_ptr<Piece> piece = Chess::getPieceFromPosition(newPos, pieceList);
			if (piece->color != color) {
				if (isValidCapture(newPos, pieceList, variant, atomicKings, checksEnabled)) {
					addCaptureSquare(newPos);
					piece->addAttacker();
				}
			}
			break;
		}
		else {
			if (isValidMove(newPos, pieceList, variant, atomicKings, checksEnabled)) {
				addMoveSquare(newPos);
			}
		}
	}
	// Left
	for (int i = 1; i < 8; i++) {
		int newX = position.x - i, newY = position.y;
		if (newX < 1) { break; }
		sf::Vector2i newPos = { newX, newY };
		if (Chess::isPieceAt(newPos, pieceList)) {
			std::shared_ptr<Piece> piece = Chess::getPieceFromPosition(newPos, pieceList);
			if (piece->color != color) {
				if (isValidCapture(newPos, pieceList, variant, atomicKings, checksEnabled)) {
					addCaptureSquare(newPos);
					piece->addAttacker();
				}
			}
			break;
		}
		else {
			if (isValidMove(newPos, pieceList, variant, atomicKings, checksEnabled)) {
				addMoveSquare(newPos);
			}

		}
	}
	if (!availablePositions.empty()) {
		canMove = true;
	}
	if (!availableCapturePositions.empty()) {
		canMove = true;
	}
}

bool Rook::validatePosition(const pieceVector& pieceList)
{
	// Up
	for (int i = 1; i < 8; i++) {
		int newX = position.x, newY = position.y + i;
		if (newY > 8) { break; }
		sf::Vector2i newPos = { newX, newY };
		if (Chess::isPieceAt(newPos, pieceList)) {
			std::shared_ptr<Piece> p = Chess::getPieceFromPosition(newPos, pieceList);
			if (p->hasID('k') && p->color != color) {
				return false;
			}
			else {
				break;
			}
		}
	}
	// Down
	for (int i = 1; i < 8; i++) {
		int newX = position.x, newY = position.y - i;
		if (newY < 1) { break; }
		sf::Vector2i newPos = { newX, newY };
		if (Chess::isPieceAt(newPos, pieceList)) {
			std::shared_ptr<Piece> p = Chess::getPieceFromPosition(newPos, pieceList);
			if (p->hasID('k') && p->color != color) {
				return false;
			}
			else {
				break;
			}
		}
	}
	// Right
	for (int i = 1; i < 8; i++) {
		int newX = position.x + i, newY = position.y;
		if (newX > 8) { break; }
		sf::Vector2i newPos = { newX, newY };
		if (Chess::isPieceAt(newPos, pieceList)) {
			std::shared_ptr<Piece> p = Chess::getPieceFromPosition(newPos, pieceList);
			if (p->hasID('k') && p->color != color) {
				return false;
			}
			else {
				break;
			}
		}
	}
	// Left
	for (int i = 1; i < 8; i++) {
		int newX = position.x - i, newY = position.y;
		if (newX < 1) { break; }
		sf::Vector2i newPos = { newX, newY };
		if (Chess::isPieceAt(newPos, pieceList)) {
			std::shared_ptr<Piece> p = Chess::getPieceFromPosition(newPos, pieceList);
			if (p->hasID('k') && p->color != color) {
				return false;
			}
			else {
				break;
			}
		}
	}
	return true;
}
