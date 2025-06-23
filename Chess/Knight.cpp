#include "Knight.h"

Knight::Knight(int x, int y, float scale, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, Chess::PColor color, sf::Texture& texture, bool animated, bool promoted, bool reversed)
	: Piece(x, y, scale, boardOffset, boardSize, boardMultiplier, color, texture, animated, promoted, reversed)
{
	name = "Knight";
	id = 'n';
	pointValue = 3;

}

Knight::~Knight()
{
}

std::shared_ptr<Piece> Knight::clone() const
{
	return std::make_shared<Knight>(*this);
}

void Knight::generateLegalMoves(const pieceVector& pieceList, Chess::Variant variant, bool atomicKings, bool checksEnabled, bool castlingEnabled, bool chess960, bool hasDoubleCheck)
{
	canMove = false;
	availablePositions->clear();
	availableCapturePositions->clear();
	if (!hasDoubleCheck) {
		for (int i = 0; i < 8; i++) {
			sf::Vector2i newPos = { this->getLocalPos().x + offsets[i].x, this->getLocalPos().y + offsets[i].y };
			if (Chess::isValidSquare(newPos)) {
				if (Chess::isPieceAt(newPos, pieceList)) {
					std::shared_ptr<Piece> piece = Chess::getPieceFromPosition(newPos, pieceList);
					if (piece->color != color) {
						if (isValidCapture(newPos, pieceList, variant, atomicKings, checksEnabled)) {
							addCaptureSquare(newPos);
							piece->addAttacker();
						}
					}
				}
				else {
					if (isValidMove(newPos, pieceList, variant, atomicKings, checksEnabled)) {
						addMoveSquare(newPos);
					}
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

bool Knight::validatePosition(const pieceVector& pieceList)
{
	for (int i = 0; i < 8; i++) {
		sf::Vector2i newPos = { this->getLocalPos().x + offsets[i].x, this->getLocalPos().y + offsets[i].y };
		if (Chess::isValidSquare(newPos)) {
			if (Chess::isPieceAt(newPos, pieceList)) {
				std::shared_ptr<Piece> p = Chess::getPieceFromPosition(newPos, pieceList);
				if (p->IsA("King") && p->color != color) {
					return false;
				}
			}
		}
	}
	return true;
}
