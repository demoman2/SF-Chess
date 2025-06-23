#include "Board.h"

// Calculating
void Board::generateLegalMoves(bool init) {
	cClock.start();
	if (dropsEnabled) {
		for (auto& piece : whiteDropPieces) {
			piece.update();
		}
		for (auto& piece : blackDropPieces) {
			piece.update();
		}
	}
	if (checksEnabled) {
		hasDoubleCheck = false;
		atomicKings = false;
		if (variant == Chess::Atomic) {
			for (int i = 0; i < pieceList.size(); i++) {
				if (pieceList.at(i)->IsA("King")) {
					std::shared_ptr<Piece>& king1 = pieceList.at(i);
					for (int x = -1; x <= 1; x++) {
						for (int y = -1; y <= 1; y++) {
							if (x != 0 || y != 0) {
								sf::Vector2i newPos = { king1->getLocalPos().x + x, king1->getLocalPos().y + y };
								if (Chess::isValidSquare(newPos)) {
									if (Chess::isPieceAt(newPos, pieceList) && getPieceFromCurrentPosition(newPos)->IsA("King")) {
										atomicKings = true;
										break;
									}
								}
							}
						}
					}
				}
			}
		}
		for (auto& p : pieceList) {
			if (p->IsA("King")) {
				std::shared_ptr<King> k = std::dynamic_pointer_cast<King>(p);
				k->inCheck = false;
				k->inDoubleCheck = false;
			}
		}
		for (auto& p : pieceList) {
			if (whiteToPlay != p->isWhite()) {
				if (p->name != "King") {
					p->generateLegalMoves(pieceList, variant, atomicKings, checksEnabled, castlingEnabled, chess960Enabled, hasDoubleCheck);
				}
			}
		}
		if (!atomicKings) {
			for (auto& p : pieceList) {
				if (p->IsA("King") && (whiteToPlay == (p->color == Chess::PColor::White))) {
					std::shared_ptr<King> k = std::dynamic_pointer_cast<King>(p);
					if (k->inCheck) {
						hasCheck = true;
						checkSprite.setPosition(getGlobalPosition(k->getLocalPos()));
						if (!init) {
							if (whiteToPlay) {
								whiteChecks++;
							}
							else {
								blackChecks++;
							}
						}
						if (variant == Chess::Atomic) { break; }
					}
					if (k->inDoubleCheck) { hasDoubleCheck = true; }
					break;
				}
			}
		}
		for (auto& p : pieceList) {
			if (whiteToPlay == p->isWhite()) {
				if (p->name != "King") {
					p->generateLegalMoves(pieceList, variant, atomicKings, checksEnabled, castlingEnabled, chess960Enabled, hasDoubleCheck);
				}
			}
		}
		for (int i = 0; i < pieceList.size(); i++) {
			if (pieceList.at(i)->IsA("King")) {
				pieceList.at(i)->generateLegalMoves(pieceList, variant, atomicKings, checksEnabled, castlingEnabled, chess960Enabled, hasDoubleCheck);
			}
		}
	}
	else {
		for (int i = 0; i < pieceList.size(); i++) {
			if (pieceList.at(i)->name != "King") {
				pieceList.at(i)->generateLegalMoves(pieceList, variant, atomicKings, checksEnabled, castlingEnabled, chess960Enabled, hasDoubleCheck);
			}
		}
		for (int i = 0; i < pieceList.size(); i++) {
			if (pieceList.at(i)->IsA("King")) {
				pieceList.at(i)->generateLegalMoves(pieceList, variant, atomicKings, checksEnabled, castlingEnabled, chess960Enabled, hasDoubleCheck);
			}
		}
	}
	if (variant == Chess::Antichess) {
		bool hasCapture = false;
		for (auto& p : pieceList) {
			if (p != nullptr && whiteToPlay == p->isWhite()) {
				if (!p->getCaptureSquares()->empty()) {
					hasCapture = true;
					break;
				}
				else if (p->IsA("Pawn")) {
					std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(p);
					if (!pawn->getEnPassantSquares()->empty()) {
						hasCapture = true;
						break;
					}
				}
			}
		}
		if (hasCapture) {
			for (auto& p : pieceList) {
				if (whiteToPlay == p->isWhite()) {
					p->getMoveSquares()->clear();
				}
			}
		}
	}
	if (dropsEnabled) {
		calculateDropPositions();
	}
	std::cout << "Finished Calculating in: " << cClock.reset().asMilliseconds() << "ms." << std::endl;
	cClock.stop();
	setExtraSprites();
	generatingMoves = false;
}

bool Board::intersects(sf::Vector2f position)
{
	return boardSprite.getGlobalBounds().contains(position) || mouseSelecting;
}

void Board::calculateDropPositions() {
	dropSquares.clear();
	std::vector<sf::Texture>& bT = *boardTextures;
	bool hasCheck = false;
	for (auto& piece : pieceList) {
		if (piece->IsA("King")) {
			std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(piece);
			if (king->inCheck) {
				hasCheck = true;
				break;
			}
		}
	}
	for (int x = 1; x <= 8; x++) {
		for (int y = 1; y <= 8; y++) {
			if (getPieceFromCurrentPosition({ x, y }) == nullptr) {
				if (checksEnabled && hasCheck) {
					Chess::PColor col = whiteToPlay ? Chess::White : Chess::Black;
					pieceVector newV;
					for (size_t j = 0; j < pieceList.size(); j++)
					{
						newV.push_back(pieceList[j]->clone());
					}
					newV.push_back(std::make_shared<Knight>(x, y, pieceScale, boardOffset, boardSize, boardMultiplier, col, pieceTextures.at(0), false, false, true));
					if (isValidPosition(newV, col)) {
						sf::Sprite square{ bT.at(0) };
						square.setOrigin(square.getLocalBounds().getCenter());
						square.setScale(boardSprite.getScale());
						square.setPosition(getGlobalPosition(sf::Vector2i{ x, y }));
						dropSquares.emplace_back(square, getLocalPosition(square.getPosition()));
					}
				}
				else {
					sf::Sprite square{ bT.at(0) };
					square.setOrigin(square.getLocalBounds().getCenter());
					square.setScale(boardSprite.getScale());
					square.setPosition(getGlobalPosition(sf::Vector2i{ x, y }));
					dropSquares.emplace_back(square, getLocalPosition(square.getPosition()));
				}
			}
		}
	}
}

Chess::Endgame Board::determineEnd() {
	// Neither 0, Stalemate 1, Checkmate 2, Insufficient Material 3, 50 Move Rule 4, Threefold Repeition 5
	if (halfMoves >= 50) {
		return Chess::FiftyMoveRule;
	}
	for (auto& p : allPositionsPlayed) {
		if (std::count(allPositionsPlayed.begin(), allPositionsPlayed.end(), p) >= 3) { return Chess::ThreefoldRepetition; }
	}
	if (variant == Chess::Standard) {
		if (std::none_of(pieceList.begin(), pieceList.end(), [whiteToPlay = this->whiteToPlay](std::shared_ptr<Piece>& piece) { return (whiteToPlay == piece->isWhite()) && piece->canMove; })) {
			for (auto& piece : pieceList) {
				if (whiteToPlay == (piece->color == Chess::PColor::White) && piece->IsA("King")) {
					std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(piece);
					if (king->inCheck) {
						return Chess::Checkmate;
					}
					else {
						return Chess::Stalemate;
					}
				}
			}
		}
		else {
			if (pieceList.size() == 2) {
				return Chess::InsufficientMaterial;
			}
			if (pieceList.size() == 3) {
				if (std::none_of(pieceList.begin(), pieceList.end(), [](std::shared_ptr<Piece>& piece) { return piece->IsA("Rook") || piece->IsA("Queen") || piece->IsA("Pawn"); })) {
					return Chess::InsufficientMaterial;
				}
			}
			else if (pieceList.size() == 4) {
				for (auto& piece : pieceList) {
					if (piece->IsA("Pawn")) {
						return Chess::None;
					}
				}
				int whiteCount = 0, blackCount = 0;
				for (auto& piece : pieceList) {
					if (piece->isWhite()) {
						whiteCount++;
					}
					else { blackCount++; }
				}
				if (whiteCount != blackCount) {
					if (std::none_of(pieceList.begin(), pieceList.end(), [](std::shared_ptr<Piece>& piece) { return piece->IsA("Rook") || piece->IsA("Queen"); })) {
						int knights = 0;
						for (auto& piece : pieceList) {
							if (piece->name == "Knight") {
								knights++;
							}
						}
						if (knights == 2) {
							return Chess::InsufficientMaterial;
						}
					}
				}
				else {
					if (std::none_of(pieceList.begin(), pieceList.end(), [](std::shared_ptr<Piece>& piece) { return piece->IsA("Rook") || piece->IsA("Queen") || piece->IsA("Pawn"); })) {
						return Chess::InsufficientMaterial;
					}
				}
			}
		}
	}
	else if (variant == Chess::Crazyhouse) {
		if (std::none_of(pieceList.begin(), pieceList.end(), [whiteToPlay = this->whiteToPlay](std::shared_ptr<Piece>& piece) { return (whiteToPlay == piece->isWhite()) && piece->canMove; })) {
			for (auto& piece : pieceList) {
				if (whiteToPlay == (piece->color == Chess::PColor::White) && piece->IsA("King")) {
					std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(piece);
					if (king->inCheck) {
						return Chess::Checkmate;
					}
					else {
						if (dropSquares.empty() || (whiteToPlay && whiteDropPieces.empty()) || (!whiteToPlay && blackDropPieces.empty())) {
							return Chess::Stalemate;
						}
					}
				}
			}
		}
		else {
			if (whiteDropPieces.empty() && blackDropPieces.empty()) {
				if (pieceList.size() == 2) {
					return Chess::InsufficientMaterial;
				}
				if (pieceList.size() == 3) {
					if (std::none_of(pieceList.begin(), pieceList.end(), [](std::shared_ptr<Piece>& piece) { return piece->IsA("Rook") || piece->IsA("Queen") || piece->IsA("Pawn"); })) {
						return Chess::InsufficientMaterial;
					}
				}
				else if (pieceList.size() == 4) {
					for (auto& piece : pieceList) {
						if (piece->IsA("Pawn")) {
							return Chess::None;
						}
					}
					int whiteCount = 0, blackCount = 0;
					for (auto& piece : pieceList) {
						if (piece->isWhite()) {
							whiteCount++;
						}
						else { blackCount++; }
					}
					if (whiteCount != blackCount) {
						if (std::none_of(pieceList.begin(), pieceList.end(), [](std::shared_ptr<Piece>& piece) { return piece->IsA("Rook") || piece->IsA("Queen"); })) {
							int knights = 0;
							for (auto& piece : pieceList) {
								if (piece->name == "Knight") {
									knights++;
								}
							}
							if (knights == 2) {
								return Chess::InsufficientMaterial;
							}
						}
					}
					else {
						if (std::none_of(pieceList.begin(), pieceList.end(), [](std::shared_ptr<Piece>& piece) { return piece->IsA("Rook") || piece->IsA("Queen") || piece->IsA("Pawn"); })) {
							return Chess::InsufficientMaterial;
						}
					}
				}
			}
		}
	}
	else if (variant == Chess::Antichess) {
		if (std::none_of(pieceList.begin(), pieceList.end(), [whiteToPlay = this->whiteToPlay](std::shared_ptr<Piece>& piece) { return (whiteToPlay == piece->isWhite()); })) {
			return Chess::Checkmate;
		}
		if (std::none_of(pieceList.begin(), pieceList.end(), [whiteToPlay = this->whiteToPlay](std::shared_ptr<Piece>& piece) { return (whiteToPlay == piece->isWhite()) && piece->canMove; })) {
			return Chess::Checkmate;
		}
		if (pieceList.size() == 2) {
			if (std::all_of(pieceList.begin(), pieceList.end(), [](std::shared_ptr<Piece>& piece) { return piece->IsA("Bishop"); })) {
				int n = pieceList.at(0)->getLocalPos().x + pieceList.at(0)->getLocalPos().y % 2;
				if (pieceList.at(1)->getLocalPos().x + pieceList.at(1)->getLocalPos().y % 2 != n) {
					return Chess::InsufficientMaterial;
				}
			}
		}
	}
	else if (variant == Chess::Atomic) {
		if (std::none_of(pieceList.begin(), pieceList.end(), [whiteToPlay = this->whiteToPlay](std::shared_ptr<Piece>& piece) { return (whiteToPlay == piece->isWhite()) && piece->IsA("King"); })) {
			return Chess::Checkmate;
		}
		if (std::none_of(pieceList.begin(), pieceList.end(), [whiteToPlay = this->whiteToPlay](std::shared_ptr<Piece>& piece) { return (whiteToPlay == piece->isWhite()) && piece->canMove; })) {
			for (auto& piece : pieceList) {
				if (whiteToPlay == (piece->color == Chess::PColor::White) && piece->IsA("King")) {
					std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(piece);
					if (king->inCheck) {
						return Chess::Checkmate;
					}
					else {
						return Chess::Stalemate;
					}
				}
			}
		}
	}
	else if (variant == Chess::Horde) {
		if (!whiteToPlay) {
			if (std::none_of(pieceList.begin(), pieceList.end(), [whiteToPlay = this->whiteToPlay](std::shared_ptr<Piece>& piece) { return (whiteToPlay == piece->isWhite()) && piece->canMove; })) {
				for (auto& piece : pieceList) {
					if (whiteToPlay == (piece->color == Chess::PColor::White) && piece->IsA("King")) {
						std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(piece);
						if (king->inCheck) {
							return Chess::Checkmate;
						}
						else {
							return Chess::Stalemate;
						}
					}
				}
			}
		}
		else {
			if (std::none_of(pieceList.begin(), pieceList.end(), [whiteToPlay = this->whiteToPlay](std::shared_ptr<Piece>& piece) { return (whiteToPlay == piece->isWhite()); })) {
				return Chess::Checkmate;
			}
			if (std::none_of(pieceList.begin(), pieceList.end(), [whiteToPlay = this->whiteToPlay](std::shared_ptr<Piece>& piece) { return (whiteToPlay == piece->isWhite()) && piece->canMove; })) {
				return Chess::Stalemate;
			}
		}
	}
	else if (variant == Chess::ThreeCheck) {
		if (whiteToPlay) {
			if (whiteChecks >= 3) {
				return Chess::VariantLoss;
			}
		}
		else {
			if (blackChecks >= 3) {
				return Chess::VariantLoss;
			}
		}
	}
	else if (variant == Chess::FiveCheck) {
		if (whiteToPlay) {
			if (whiteChecks >= 5) {
				return Chess::VariantLoss;
			}
		}
		else {
			if (blackChecks >= 5) {
				return Chess::VariantLoss;
			}
		}
	}
	else if (variant == Chess::KOTH) {
		if (std::none_of(pieceList.begin(), pieceList.end(), [whiteToPlay = this->whiteToPlay](std::shared_ptr<Piece>& piece) { return (whiteToPlay == piece->isWhite()) && piece->canMove; })) {
			for (auto& piece : pieceList) {
				if (whiteToPlay == (piece->color == Chess::PColor::White) && piece->IsA("King")) {
					std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(piece);
					if (king->inCheck) {
						return Chess::Checkmate;
					}
					else {
						return Chess::Stalemate;
					}
				}
			}
		}
		if (std::any_of(pieceList.begin(), pieceList.end(), [whiteToPlay = this->whiteToPlay](std::shared_ptr<Piece>& piece) { return (whiteToPlay != piece->isWhite()) && piece->IsA("King") && Chess::isInCenter(piece->getLocalPos()); })) {
			return Chess::Checkmate;
		}
	}
	else if (variant == Chess::RacingKings) {
		if (std::none_of(pieceList.begin(), pieceList.end(), [whiteToPlay = this->whiteToPlay](std::shared_ptr<Piece>& piece) { return (whiteToPlay == piece->isWhite()) && piece->canMove; })) {
			return Chess::Stalemate;
		}
		bool white = false, black = false;
		for (auto& piece : pieceList) {
			if (piece->IsA("King") && piece->isWhite() && piece->getLocalPos().y == 8) {
				white = true;
				if (black) { break; }
			}
			if (piece->IsA("King") && piece->isBlack() && piece->getLocalPos().y == 8) {
				black = true;
				if (white) { break; }
			}
		}
		if (white && black) {
			return Chess::Stalemate;
		}
		if (black) {
			return Chess::VariantLoss;
		}
		if (white) {
			for (auto& piece : pieceList) {
				if (piece->IsA("King") && piece->isBlack()) {
					if (std::any_of(piece->getMoveSquares()->begin(), piece->getMoveSquares()->end(), [](Chess::Square& square) { return square.pos.y == 8; }) ||
						std::any_of(piece->getCaptureSquares()->begin(), piece->getCaptureSquares()->end(), [](Chess::Square& square) { return square.pos.y == 8; })) {
						if (!eighthRankWhite) {
							eighthRankWhite = true;
							return Chess::None;
						}
						else {
							return Chess::VariantVictory;
						}
					}
					else {
						if (whiteToPlay) {
							return Chess::VariantVictory;
						}
						else {
							return Chess::VariantLoss;
						}
					}
				}
			}
		}
	}
	return Chess::None;
}

void Board::setVariant(Chess::Variant variant)
{
	this->variant = variant;
	setupFEN({}, true);
}

// Move Functions
void Board::playMove(std::string moveString, bool instantMove) {

	playingMove = true;
	if (moveString.empty()) {
		std::cout << "Got Empty Move String!" << std::endl;
		return;
	}
	else {
		hasCheck = false;
		selectedPiece.reset();

		if (moveString.back() == '\r') { std::cout << "Game Should End" << std::endl; gameShouldEnd = true; moveString.pop_back(); }

		// Drop Pieces
		if (moveString.at(1) == '@') {
			moves += " " + moveString;
			sf::Vector2i dest = Chess::convertChessNotationtoPosition(moveString.substr(2, 2));
			Chess::PColor col = whiteToPlay ? Chess::PColor::White : Chess::PColor::Black;
			int offset = whiteToPlay ? 0 : -6;
			moveSound.play();
			lastMoveStart.setPosition({ -1000, -1000 });
			lastMoveDest.setPosition(getGlobalPosition(dest));
			if (whiteToPlay) {
				for (auto& piece : whiteDropPieces) {
					if (piece.id == std::tolower(moveString.at(0))) {
						piece.count--;
						break;
					}
				}
			}
			else {
				for (auto& piece : blackDropPieces) {
					if (piece.id == std::tolower(moveString.at(0))) {
						piece.count--;
						break;
					}
				}
			}

			std::shared_ptr<Piece> newPiece = getPieceFromID(moveString.front(), dest, col, false);
			pieceList.push_back(newPiece);
			calculatingPos = true;
			std::thread postMoveF(&Board::postMove, this, pieceVector{ newPiece });
			postMoveF.detach();
		}

		else {
			int midPos = 0;
			for (int i = 0; i < moveString.size() - 1; i++) {
				if (i != 0 && !std::isdigit(moveString.at(i)) && i != (moveString.size() - 1)) {
					midPos = i;
				}
			}
			sf::Vector2i start = Chess::convertChessNotationtoPosition(moveString.substr(0, midPos));
			sf::Vector2i dest;
			std::shared_ptr<Piece> piece = getPieceFromCurrentPosition(start);
			if (piece == nullptr) {
				std::cout << "Piece was not found!" << std::endl;
				return;
			}
			std::shared_ptr<Piece> capture = nullptr;

			// Promotion
			if (!std::isdigit(moveString.back())) {
				moves += " " + moveString;
				std::string t{ moveString.begin() + midPos, moveString.end() - 1 };
				dest = Chess::convertChessNotationtoPosition(t);
				capture = getPieceFromCurrentPosition(dest);
				lastMoveStart.setPosition(getGlobalPosition(start));
				lastMoveDest.setPosition(getGlobalPosition(dest));
				if (capture != nullptr && capture->color == piece->color) {
					capture.reset();
				}

				for (size_t i = 0; i < pieceList.size(); i++) {
					if (pieceList.at(i)->getLocalPos() == start) {
						pieceList.erase(pieceList.begin() + i);
						break;
					}
				}

				if (capture != nullptr) {
					captureSound.play();
					ghostSprite.setTexture(capture->getTexture());
					ghostSprite.setPosition(capture->getGlobalPos());
					setSpriteVisible(ghostSprite, true, 75);
					capture->setVisible(false);
					capturePiece = capture;
				}
				else {
					moveSound.play();
				}

				std::shared_ptr<Piece> newPiece = getPieceFromID(moveString.back(), dest, piece->color, true);
				pieceList.push_back(newPiece);
				calculatingPos = true;
				std::thread postMoveF(&Board::postMove, this, pieceVector{ newPiece });
				postMoveF.detach();
			}

			else {
				dest = Chess::convertChessNotationtoPosition(moveString.substr(midPos));
				capture = getPieceFromCurrentPosition(dest);
				bool closeCastle = false;
				if (capture != nullptr) {
					if (capture->color == piece->color) {
						if (piece->IsA("King") && capture->IsA("Rook")) {
							closeCastle = true;
						}
						capture.reset();
					}
				}

				piece->setTarget(getGlobalPosition(dest));
				if (instantMove) {
					piece->setPosition(piece->getTarget());
				}
				if (piece->IsA("Pawn")) {
					if (piece->color == Chess::PColor::White) {
						if (dest.y == piece->getLocalPos().y + 2) {
							std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(piece);
							pawn->enPassantTarget = true;
						}
						if (dest.x != piece->getLocalPos().x && capture == nullptr) {
							std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(piece);
							for (auto& piece : pieceList) {
								if (piece->getLocalPos() == (dest - sf::Vector2i(0, 1))) {
									capture = piece;
									break;
								}
							}
						}
						if (dest.y == 8) {
							promotePiece = piece;
							shouldPostMove = false;
						}
					}
					else {
						if (dest.y == piece->getLocalPos().y - 2) {
							std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(piece);
							pawn->enPassantTarget = true;
						}
						if (dest.x != piece->getLocalPos().x && capture == nullptr) {
							std::shared_ptr<Pawn> pawn = std::dynamic_pointer_cast<Pawn>(piece);
							for (auto& piece : pieceList) {
								if (piece->getLocalPos() == (dest + sf::Vector2i(0, 1))) {
									capture = piece;
									break;
								}
							}
						}
						if (dest.y == 1) {
							promotePiece = piece;
							shouldPostMove = false;
						}
					}
				}
				// Castles
				else if (piece->IsA("King")) {
					std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(piece);
					int pieceX = piece->getLocalPos().x;
					if (!closeCastle) {
						if (dest.x > pieceX + 1) {
							std::shared_ptr<Piece> rook = getPieceFromCurrentPosition({ king->Krook, piece->getLocalPos().y });
							if (rook != nullptr) {
								rook->setTarget(getGlobalPosition(sf::Vector2i{ 6, piece->getLocalPos().y }));
								piece->setTarget(getGlobalPosition(sf::Vector2i{ 7, piece->getLocalPos().y }));
								if (instantMove) {
									rook->setPosition(rook->getTarget());
									piece->setPosition(piece->getTarget());
								}
								castleKing = piece;
								castleRook = rook;
							}
						}
						else if (dest.x < pieceX - 1) {
							std::shared_ptr<Piece> rook = getPieceFromCurrentPosition({ king->Qrook, piece->getLocalPos().y });
							if (rook != nullptr) {
								rook->setTarget(getGlobalPosition(sf::Vector2i{ 4, piece->getLocalPos().y }));
								piece->setTarget(getGlobalPosition(sf::Vector2i{ 3, piece->getLocalPos().y }));
								if (instantMove) {
									rook->setPosition(rook->getTarget());
									piece->setPosition(piece->getTarget());
								}
								castleKing = piece;
								castleRook = rook;
							}
						}
					}
					else {
						if (dest.x > pieceX) {
							std::shared_ptr<Piece> rook = getPieceFromCurrentPosition({ king->Krook, piece->getLocalPos().y });
							if (rook != nullptr) {
								rook->setTarget(getGlobalPosition(sf::Vector2i{ 6, piece->getLocalPos().y }));
								piece->setTarget(getGlobalPosition(sf::Vector2i{ 7, piece->getLocalPos().y }));
								if (instantMove) {
									rook->setPosition(rook->getTarget());
									piece->setPosition(piece->getTarget());
								}
								castleKing = piece;
								castleRook = rook;
							}
						}
						else if (dest.x < pieceX) {
							std::shared_ptr<Piece> rook = getPieceFromCurrentPosition({ king->Qrook, piece->getLocalPos().y });
							if (rook != nullptr) {
								rook->setTarget(getGlobalPosition(sf::Vector2i{ 4, piece->getLocalPos().y }));
								piece->setTarget(getGlobalPosition(sf::Vector2i{ 3, piece->getLocalPos().y }));
								if (instantMove) {
									rook->setPosition(rook->getTarget());
									piece->setPosition(piece->getTarget());
								}
								castleKing = piece;
								castleRook = rook;
							}
						}
					}
				}

				if (capture != nullptr) {
					ghostSprite.setTexture(capture->getTexture());
					ghostSprite.setPosition(capture->getGlobalPos());
					setSpriteVisible(ghostSprite, true, 75);
					capture->setVisible(false);
					capturePiece = capture;
				}

				if (promotePiece == nullptr) {
					moves += " " + moveString;
					lastMoveStart.setPosition(getGlobalPosition(piece->getLocalPos()));
					lastMoveDest.setPosition(getGlobalPosition(dest));
					if (capture != nullptr) {
						captureSound.play();
					}
					else {
						moveSound.play();
					}
				}

			}
		}
	}
	playingMove = false;
}

void Board::postMove(pieceVector movePieces) {
	whiteToPlay = !whiteToPlay;
	hasCheck = false;
	lastMoveStartLocal = getLocalPosition(lastMoveStart.getPosition());
	lastMoveDestLocal = getLocalPosition(lastMoveDest.getPosition());
	halfMoves++;

	if (movePieces.empty()) { calculatingPos = false; return; }
	else if (movePieces.front()->isBlack()) { fullMoves++; }

	// Captures
	if (capturePiece != nullptr) {
		halfMoves = 0;
		setSpriteVisible(ghostSprite, false);
		std::shared_ptr<Piece>& piece = movePieces.front();
		sf::Vector2i cpos = capturePiece->getLocalPos();
		for (auto it = pieceList.begin(); it != pieceList.end(); it++) {
			if (*it == capturePiece) {
				pieceList.erase(it);
				break;
			}
		}
		if (variant == Chess::Atomic) {
			std::vector<sf::Vector2i> piecePositions{ piece->getLocalPos() };
			for (int x = -1; x <= 1; x++) {
				for (int y = -1; y <= 1; y++) {
					sf::Vector2i newPos = { cpos.x + x, cpos.y + y };
					if (Chess::isValidSquare(newPos)) {
						if (Chess::isPieceAt(newPos, pieceList)) {
							if (x != 0 || y != 0) {
								std::shared_ptr<Piece> p = getPieceFromCurrentPosition(newPos);
								if (p->name != "Pawn") {
									piecePositions.push_back(newPos);
								}
							}
						}
						sf::RectangleShape atomicRect{ {boardMultiplier, boardMultiplier } };
						atomicRect.setOrigin(atomicRect.getLocalBounds().getCenter());
						atomicRect.setPosition(getGlobalPosition(newPos));
						atomicRect.setFillColor(sf::Color{ 255, 255, 255, 150 });
						captureObjects.push_back(atomicRect);
					}
				}
			}
			for (auto& pos : piecePositions) {
				for (auto it = pieceList.begin(); it != pieceList.end(); it++) {
					std::shared_ptr<Piece> p = *it;
					if (p->getLocalPos() == pos) {
						pieceList.erase(it);
						break;
					}
				}
			}
			atomicClock.start();
		}

		if (dropsEnabled) {
			if (piece->isWhite()) {
				if (capturePiece->promoted) {
					for (auto& t : whiteDropPieces) {
						if (t.id == 'p') {
							t.count += 1;
							break;
						}
					}
				}
				else {
					for (auto& t : whiteDropPieces) {
						if (t.id == capturePiece->id) {
							t.count += 1;
							break;
						}
					}
				}
			}
			else {
				if (capturePiece->promoted) {
					for (auto& t : blackDropPieces) {
						if (t.id == 'p') {
							t.count += 1;
							break;
						}
					}
				}
				else {
					for (auto& t : blackDropPieces) {
						if (t.id == capturePiece->id) {
							t.count += 1;
							break;
						}
					}
				}
			}
		}
		capturePiece.reset();
	}

	// Set Local Pos
	for (auto& mPiece : movePieces) {
		mPiece->setTarget({});
		mPiece->setLocalPosition(getLocalPosition(mPiece->getGlobalPos()));
		mPiece->setPosition(mPiece->getGlobalPos());
		if (!mPiece->hasMoved) { mPiece->hasMoved = true; }
		if (mPiece->IsA("Pawn")) { halfMoves = 0; }
	}
	for (auto& p : pieceList) {
		if (std::none_of(movePieces.begin(), movePieces.end(), [p](auto& mPiece) { return mPiece == p; })) {
			p->afterMovePlayed();
		}
	}

	if (autoFlip && (!stockfishEnabled || stockfishEnabled && isPaused) && whiteToPlay == isFlipped) {
		flipBoard();
	}

	generateLegalMoves(false);
	allPositionsPlayed.push_back(savePosition());

	std::string sideToPlayString = whiteToPlay ? "White " : "Black ";
	if (determineEnd() != Chess::None) {
		switch (determineEnd()) {
		case Chess::Endgame::Checkmate:
		{
			std::cout << sideToPlayString << "Lost to Checkmate!" << std::endl;
			break;
		}
		case Chess::Endgame::VariantVictory:
		{
			std::cout << sideToPlayString << "Won! Variant Ending." << std::endl;
			break;
		}
		case Chess::Endgame::VariantLoss:
		{
			std::cout << sideToPlayString << "Lost! Variant Ending." << std::endl;
			break;
		}
		case Chess::Endgame::Stalemate:
		{
			std::cout << "Game Ended in Stalemate!" << std::endl;
			break;
		}
		case Chess::Endgame::ThreefoldRepetition:
		{
			std::cout << "Game Ended by Threefold Repetition!" << std::endl;
			break;
		}
		case Chess::Endgame::FiftyMoveRule:
		{
			std::cout << "Game Ended by 50 Move Rule!" << std::endl;
			break;
		}
		}
		gameEnded = true;
	}
	else if (gameShouldEnd) {
		isPaused = true;
	}

	if (!gameEnded) {
		int moveCount = getMoveCount();
		int n = stockfish.getLegalMoveCount(FEN, moves);
		if (n != moveCount && n != 0) {
			std::cout << "===========================MISMATCH===========================" << std::endl;
			std::cout << "Count: " << moveCount << std::endl;
			std::cout << n << std::endl;
			isPaused = true;
		}
	}

	calculatingPos = false;
}