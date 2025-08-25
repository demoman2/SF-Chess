#include "King.h"

King::King(int x, int y, float scale, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, Chess::PColor color, sf::Texture& texture, bool animated, bool promoted, bool reversed)
	: Piece(x, y, scale, boardOffset, boardSize, boardMultiplier, color, texture, animated, promoted, reversed), canCastleK(false), canCastleQ(false), canNeverCastleK(false), canNeverCastleQ(false), inCheck(false), Krook(-1), Qrook(-1)
{
	positionVectors.push_back(&castlePositions);
	positionVectors.push_back(&captureCastlePositions);
	id = 'k';
}

King::King(bool canNeverCastleK, bool canNeverCastleQ, int x, int y, float scale, sf::Vector2f boardOffset, sf::Vector2f boardSize, float boardMultiplier, Chess::PColor color, sf::Texture& texture, bool animated, bool promoted, bool reversed)
	: Piece(x, y, scale, boardOffset, boardSize, boardMultiplier, color, texture, animated, promoted, reversed), canCastleK(false), canCastleQ(false), inCheck(false), Krook(-1), Qrook(-1)
{
	positionVectors.push_back(&castlePositions);
	positionVectors.push_back(&captureCastlePositions);
	id = 'k';
	this->canNeverCastleK = canNeverCastleK;
	this->canNeverCastleQ = canNeverCastleQ;
}

King::King(const King& other, bool copySprite, bool copySquares) : Piece(other, copySprite, copySquares), inCheck(other.inCheck), canCastleK(other.canCastleK), canCastleQ(other.canCastleQ),
	canNeverCastleK(other.canNeverCastleK), canNeverCastleQ(other.canNeverCastleQ), Krook(other.Krook), Qrook(other.Qrook)
{
	if (copySquares) {
		castlePositions = other.castlePositions;
		captureCastlePositions = other.captureCastlePositions;
	}
	positionVectors.push_back(&castlePositions);
	positionVectors.push_back(&captureCastlePositions);
}

King::~King()
{
}

std::shared_ptr<Piece> King::clone(bool copySprite, bool copySquares) const
{
	return std::make_shared<King>(*this, copySprite, copySquares);
}

void King::addAttacker()
{
	inCheck = true;
}

bool King::isValidMove(sf::Vector2i square, const pieceVector& pieceList, Chess::Variant variant, bool atomicKings, bool checksEnabled)
{
	if (checksEnabled) {
		pieceVector newV = Chess::makePieceVec(pieceList, square, getLocalPos(), variant);
		if (variant == Chess::Atomic) {
			if (!isValidPosition(newV, color) && !enemyKingOccupies(square, pieceList, color)) {
				return false;
			}
		}
		else if (variant == Chess::RacingKings) {
			if (!isValidPosition(newV, color) || !isValidPosition(newV, !color)) {
				return false;
			}
		}
		else {
			if (!isValidPosition(newV, color)) {
				return false;
			}
		}
	}
	return true;
}

bool King::validatePosition(const pieceVector& pieceList)
{
	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			if (x != 0 || y != 0) {
				sf::Vector2i newPos = { this->getLocalPos().x + x, this->getLocalPos().y + y };
				if (Chess::isValidSquare(newPos)) {
					if (Chess::isPieceAt(newPos, pieceList)) {
						std::shared_ptr<Piece> p = Chess::getPieceFromPosition(newPos, pieceList);
						if (p->hasID('k') && p->color != color) {
							return false;
						}
					}
				}
			}
		}
	}
	return true;
}

void King::updateSprites(std::vector<sf::Texture>& boardTextures, sf::Vector2f mousePos, bool& mouseSelecting)
{
	for (auto& square : availablePositions) {
		if (square.sprite) {
			auto& sprite = square.sprite;
			if (sprite->getGlobalBounds().contains(sf::Vector2f(mousePos))) {
				mouseSelecting = true;
				sprite->setTexture(boardTextures.at(4));
			}
			else { sprite->setTexture(boardTextures.at(0)); }
		}
	}
	for (auto& square : availableCapturePositions) {
		if (square.sprite) {
			auto& sprite = square.sprite;
			if (sprite->getGlobalBounds().contains(sf::Vector2f(mousePos))) {
				mouseSelecting = true;
				sprite->setTexture(boardTextures.at(4));
			}
			else { sprite->setTexture(boardTextures.at(1)); }
		}
	}
	for (auto& square : castlePositions) {
		auto& sprite = square.sprite;
		if (sprite->getGlobalBounds().contains(sf::Vector2f(mousePos))) {
			mouseSelecting = true;
			sprite->setTexture(boardTextures.at(4));
		}
		else { sprite->setTexture(boardTextures.at(0)); }
	}
	
	for (auto& square : captureCastlePositions) {
		auto& sprite = square.sprite;
		if (sprite->getGlobalBounds().contains(sf::Vector2f(mousePos))) {
			mouseSelecting = true;
			sprite->setTexture(boardTextures.at(4));
		}
		else { sprite->setTexture(boardTextures.at(1)); }
	}
}

void King::addCastleSquare(sf::Vector2i square)
{
	std::string m = Chess::convertPositiontoNotation(position) + Chess::convertPositiontoNotation(square);
	castlePositions.emplace_back(square, m);
}

void King::addCastleCaptureSquare(sf::Vector2i square, bool chess960, bool kingside)
{
	std::string m;
	if (chess960) {
		m = Chess::convertPositiontoNotation(position) + Chess::convertPositiontoNotation(square);
	}
	else {
		if (kingside) { m = Chess::convertPositiontoNotation(position) + ("g" + std::to_string(position.y)); }
		else { m = Chess::convertPositiontoNotation(position) + ("c" + std::to_string(position.y)); }
	}
	captureCastlePositions.emplace_back(square, m);
}

void King::calculateCastlingRights(pieceVector pieceList, bool checksEnabled, bool chess960) {
	canCastleK = false;
	canCastleQ = false;
	if (!hasMoved && !inCheck) {
		// Kingside
		if (!canNeverCastleK && Krook != -1) {
			if (!chess960 && Krook != 8) { goto Queenside; }
			if (Chess::getPieceFromPosition({ Krook, position.y }, pieceList)) {
				std::shared_ptr<Piece> rook = Chess::getPieceFromPosition({ Krook, position.y }, pieceList);
				if (rook->hasID('r') && rook->color == color && !rook->hasMoved) {
					std::vector<sf::Vector2i> castleSquares, kingSquares;
					sf::Vector2i target = Chess::convertChessNotationtoPosition("g" + std::to_string(position.y));
					if (position.x > rook->getLocalPos().x) {
						for (int i = rook->getLocalPos().x + 1; i < position.x; i++) {
							castleSquares.push_back({ i, position.y });
						}
					}
					else {
						for (int i = position.x + 1; i < rook->getLocalPos().x; i++) {
							castleSquares.push_back({ i, position.y });
						}
					}
					for (int i = std::min(target.x, position.x); i <= std::max(target.x, position.x); i++) {
						kingSquares.push_back({ i , position.y });
					}
					auto find = std::find(castleSquares.begin(), castleSquares.end(), Chess::convertChessNotationtoPosition("f" + std::to_string(position.y)));
					if (find == castleSquares.end()) {
						castleSquares.push_back(Chess::convertChessNotationtoPosition("f" + std::to_string(position.y)));
					}
					auto find2 = std::find(castleSquares.begin(), castleSquares.end(), Chess::convertChessNotationtoPosition("g" + std::to_string(position.y)));
					if (find2 == castleSquares.end()) {
						castleSquares.push_back(Chess::convertChessNotationtoPosition("g" + std::to_string(position.y)));
					}
					for (const auto& sq : castleSquares) {
						if (Chess::getPieceFromPosition(sq, pieceList)) {
							if (sq != rook->getLocalPos() && sq != getLocalPos()) {
								goto Queenside;
							}
						}
					}
					for (const auto& sq : kingSquares) {
						if (Chess::getPieceFromPosition(sq, pieceList)) {
							if (sq != rook->getLocalPos() && sq != getLocalPos()) {
								goto Queenside;
							}
						}
						pieceVector newV = Chess::makePieceVec(pieceList, sq, getLocalPos());
						if (!Chess::isValidPosition(newV, color)) {
							goto Queenside;
						}
					}
					canCastleK = true;
				}
			}
		}
	Queenside:
		if (!canNeverCastleQ && Qrook != -1) {
			if (!chess960 && Qrook != 1) { return; }
			if (Chess::getPieceFromPosition({ Qrook, position.y }, pieceList)) {
				std::shared_ptr<Piece> rook = Chess::getPieceFromPosition({ Qrook, position.y }, pieceList);
				if (rook->hasID('r') && rook->color == color && !rook->hasMoved) {
					std::vector<sf::Vector2i> castleSquares, kingSquares;
					sf::Vector2i target = Chess::convertChessNotationtoPosition("c" + std::to_string(position.y));
					if (position.x > rook->getLocalPos().x) {
						for (int i = rook->getLocalPos().x + 1; i < position.x; i++) {
							castleSquares.push_back({ i, position.y });
						}
					}
					else {
						for (int i = position.x + 1; i < rook->getLocalPos().x; i++) {
							castleSquares.push_back({ i, position.y });
						}
					}
					for (int i = std::min(target.x, position.x); i <= std::max(target.x, position.x); i++) {
						kingSquares.push_back({ i , position.y });
					}
					auto find = std::find(castleSquares.begin(), castleSquares.end(), Chess::convertChessNotationtoPosition("c" + std::to_string(position.y)));
					if (find == castleSquares.end()) {
						castleSquares.push_back(Chess::convertChessNotationtoPosition("c" + std::to_string(position.y)));
					}
					auto find2 = std::find(castleSquares.begin(), castleSquares.end(), Chess::convertChessNotationtoPosition("d" + std::to_string(position.y)));
					if (find2 == castleSquares.end()) {
						castleSquares.push_back(Chess::convertChessNotationtoPosition("d" + std::to_string(position.y)));
					}
					for (const auto& sq : castleSquares) {
						if (Chess::getPieceFromPosition(sq, pieceList)) {
							if (sq != rook->getLocalPos() && sq != getLocalPos()) {
								return;
							}
						}
					}
					for (const auto& sq : kingSquares) {
						if (Chess::getPieceFromPosition(sq, pieceList)) {
							if (sq != rook->getLocalPos() && sq != getLocalPos()) {
								return;
							}
						}
						pieceVector newV = Chess::makePieceVec(pieceList, sq, getLocalPos());
						if (!Chess::isValidPosition(newV, color)) {
							return;
						}
					}
					canCastleQ = true;
				}
			}
		}
	}
	if (canCastleK) {
		sf::Vector2i capturePos = { Krook, position.y };
		pieceVector newV;
		newV.resize(pieceList.size());
		for (size_t j = 0; j < pieceList.size(); j++)
		{
			newV[j] = pieceList[j]->clone(false, false);
		}
		for (auto& v : newV) {
			// Rook
			if (v->getLocalPos() == sf::Vector2i{ Krook, position.y }) {
				v->setLocalPosition(Chess::convertChessNotationtoPosition("f" + std::to_string(position.y)));
			}
			// King
			if (v->getLocalPos() == getLocalPos()) {
				v->setLocalPosition(Chess::convertChessNotationtoPosition("g" + std::to_string(position.y)));
			}
		}
		if (checksEnabled) {
			if (!isValidPosition(newV, color)) {
				canCastleK = false;
			}
			else { canMove = true; }
		}
		newV.clear();

	}
	if (canCastleQ) {
		sf::Vector2i capturePos = { Qrook, position.y };
		pieceVector newV;
		newV.resize(pieceList.size());
		for (size_t j = 0; j < pieceList.size(); j++)
		{
			newV[j] = pieceList[j]->clone(false, false);
		}
		for (auto& v : newV) {
			// Rook
			if (v->getLocalPos() == sf::Vector2i{ Qrook, position.y }) {
				v->setLocalPosition(Chess::convertChessNotationtoPosition("d" + std::to_string(position.y)));
			}
			// King
			if (v->getLocalPos() == getLocalPos()) {
				v->setLocalPosition(Chess::convertChessNotationtoPosition("c" + std::to_string(position.y)));
			}
		}
		if (checksEnabled) {
			if (!isValidPosition(newV, color)) {
				canCastleQ = false;
			}
			else { canMove = true; }
		}
		newV.clear();

	}
}

void King::generateLegalMoves(const pieceVector& pieceList, Chess::Variant variant, bool atomicKings, bool checksEnabled, bool castlingEnabled, bool chess960)
{
	canMove = false;
	availablePositions.clear();
	availableCapturePositions.clear();
	castlePositions.clear();
	captureCastlePositions.clear();
	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			if (x != 0 || y != 0) {
				sf::Vector2i newPos = { this->getLocalPos().x + x, this->getLocalPos().y + y };
				std::shared_ptr<Piece> piece = Chess::getPieceFromPosition(newPos, pieceList);
				if (Chess::isValidSquare(newPos)) {
					if (piece == nullptr) {
						if (isValidMove(newPos, pieceList, variant, atomicKings, checksEnabled)) {
							addMoveSquare(newPos);
						}
					}
					else if (piece->color != color) {
						if (isValidCapture(newPos, pieceList, variant, atomicKings, checksEnabled)) {
							addCaptureSquare(newPos);
							piece->addAttacker();
						}
					}
				}
			}
		}
	}
	if (castlingEnabled) {
		calculateCastlingRights(pieceList, checksEnabled, chess960);
		sf::Vector2i castlePos, rookPos;
		if (canCastleK) {
			rookPos = Chess::convertChessNotationtoPosition("f" + std::to_string(position.y));
			sf::Vector2i capturePos = { Krook, position.y };
			addCastleCaptureSquare(capturePos, chess960, true);
			if (!chess960) {
				castlePos = Chess::convertChessNotationtoPosition("g" + std::to_string(position.y));
				addCastleSquare(castlePos);
			}
		}
		if (canCastleQ) {
			rookPos = Chess::convertChessNotationtoPosition("d" + std::to_string(position.y));
			sf::Vector2i capturePos = { Qrook, position.y };
			addCastleCaptureSquare(capturePos, chess960, false);
			if (!chess960) {
				castlePos = Chess::convertChessNotationtoPosition("c" + std::to_string(position.y));
				addCastleSquare(castlePos);
			}
		}
	}
	if (!availablePositions.empty()) {
		canMove = true;
	}
	else if (!availableCapturePositions.empty()) {
		canMove = true;
	}
	else if (!castlePositions.empty()) {
		canMove = true;
	}
	else if (!captureCastlePositions.empty()) {
		canMove = true;
	}
}
