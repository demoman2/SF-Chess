#include "Queen.h"

Queen::Queen(int x, int y, float scale, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, Chess::PColor color, sf::Texture& texture, bool animated, bool promoted, bool reversed)
	: Piece(x, y, scale, boardOffset, boardSize, boardMultiplier, color, texture, animated, promoted, reversed)
{
	id = 'q';
}

Queen::Queen(const Queen& other, bool copySprite, bool copySquares) : Piece(other, copySprite, copySquares)
{
}

Queen::~Queen()
{
}

std::shared_ptr<Piece> Queen::clone(bool copySprite, bool copySquares) const
{
	return std::make_shared<Queen>(*this, copySprite, copySquares);
}

void Queen::generateLegalMoves(const pieceVector& pieceList, Chess::Variant variant, bool atomicKings, bool checksEnabled, bool castlingEnabled, bool chess960)
{
	canMove = false;
	availablePositions.clear();
	availableCapturePositions.clear();
	// Top Right Diag
	for (int i = 1; i < 8; i++) {
		int newX = position.x + i, newY = position.y + i;
		if (std::max(newX, newY) > 8) { break; }
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
	// Top Left Diag
	for (int i = 1; i < 8; i++) {
		int newX = position.x - i, newY = position.y + i;
		if (std::max(newX, newY) > 8 || std::min(newX, newY) < 1) { break; }
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
	// Bottom Right Diag
	for (int i = 1; i < 8; i++) {
		int newX = position.x + i, newY = position.y - i;
		if (std::max(newX, newY) > 8 || std::min(newX, newY) < 1) { break; }
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
	// Bottom Left Diag
	for (int i = 1; i < 8; i++) {
		int newX = position.x - i, newY = position.y - i;
		if (std::min(newX, newY) < 1) { break; }
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

bool Queen::validatePosition(const pieceVector& pieceList)
{
	// Top Right Diag
	for (int i = 1; i < 8; i++) {
		int newX = position.x + i, newY = position.y + i;
		if (std::max(newX, newY) > 8) { break; }
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
	// Top Left Diag
	for (int i = 1; i < 8; i++) {
		int newX = position.x - i, newY = position.y + i;
		if (std::max(newX, newY) > 8 || std::min(newX, newY) < 1) { break; }
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
	// Bottom Right Diag
	for (int i = 1; i < 8; i++) {
		int newX = position.x + i, newY = position.y - i;
		if (std::max(newX, newY) > 8 || std::min(newX, newY) < 1) { break; }
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
	// Bottom Left Diag
	for (int i = 1; i < 8; i++) {
		int newX = position.x - i, newY = position.y - i;
		if (std::min(newX, newY) < 1) { break; }
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
