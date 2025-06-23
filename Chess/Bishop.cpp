#include "Bishop.h"

Bishop::Bishop(int x, int y, float scale, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, Chess::PColor color, sf::Texture& texture, bool animated, bool promoted, bool reversed)
	: Piece(x, y, scale, boardOffset, boardSize, boardMultiplier, color, texture, animated, promoted, reversed)
{
	name = "Bishop";
	id = 'b';
	pointValue = 3;
}

Bishop::~Bishop()
{
}

std::shared_ptr<Piece> Bishop::clone() const
{
	return std::make_shared<Bishop>(*this);
}

void Bishop::generateLegalMoves(const pieceVector& pieceList, Chess::Variant variant, bool atomicKings, bool checksEnabled, bool castlingEnabled, bool chess960, bool hasDoubleCheck)
{
	canMove = false;
	availablePositions->clear();
	availableCapturePositions->clear();
	if (!hasDoubleCheck) {
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
	}
	if (!availablePositions->empty()) {
		canMove = true;
	}
	if (!availableCapturePositions->empty()) {
		canMove = true;
	}
}

bool Bishop::validatePosition(const pieceVector& pieceList)
{
	// Top Right Diag
	for (int i = 1; i < 8; i++) {
		int newX = position.x + i, newY = position.y + i;
		if (std::max(newX, newY) > 8) { break; }
		sf::Vector2i newPos = { newX, newY };
		if (Chess::isPieceAt(newPos, pieceList)) {
			std::shared_ptr<Piece> p = Chess::getPieceFromPosition(newPos, pieceList);
			if (p->IsA("King") && p->color != color) {
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
			if (p->IsA("King") && p->color != color) {
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
			if (p->IsA("King") && p->color != color) {
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
			if (p->IsA("King") && p->color != color) {
				return false;
			}
			else {
				break;
			}
		}
	}
	return true;
}
