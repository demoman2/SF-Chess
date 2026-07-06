/*
	SF Chess, a Chess GUI which supports many chess variants
	Copyright (C) 2026  demoman2 (https://github.com/demoman2)

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Affero General Public License as
	published by the Free Software Foundation, either version 3 of the
	License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Affero General Public License for more details.

	You should have received a copy of the GNU Affero General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "Piece.h"
#include <iostream>

Piece::Piece(char id, int x, int y, float scale, sf::Vector2f boardOffset, sf::Vector2f boardSize, sf::Vector2u boardSquares, float boardMultiplier, Chess::PColor color, const sf::Texture& texture, bool animated, bool promoted, bool promotedPieceType, bool reversed)
	: sprite(std::make_unique<sf::Sprite>(texture)), position(x, y), color(color), id(std::tolower(id)), hasMoved(false), canMove(false), promoted(promoted), promotedPieceType(promotedPieceType), inCheck(false), value(3), demotedID(' ')
{
	positionVectors.push_back(&availablePositions);
	positionVectors.push_back(&availableCapturePositions);
	positionVectors.push_back(&enPassantPositions);
	positionVectors.push_back(&castlePositions);
	positionVectors.push_back(&captureCastlePositions);
	if (sprite) {
		if (animated) {
			sprite->setPosition(Chess::getGlobalPosition(sf::Vector2f{ (boardSquares.x / 2.0f) + 0.5f, (boardSquares.y / 2.0f) + 0.5f }, boardOffset, boardSize, boardMultiplier, boardSquares, reversed));
			animationTarget = Chess::getGlobalPosition(sf::Vector2i{ x, y }, boardOffset, boardSize, boardMultiplier, boardSquares, reversed);
		}
		else {
			sprite->setPosition(Chess::getGlobalPosition(sf::Vector2i{ x, y }, boardOffset, boardSize, boardMultiplier, boardSquares, reversed));
		}
		sprite->setOrigin(sprite->getLocalBounds().getCenter());
		sprite->setScale(sf::Vector2f(scale, scale));
	}
}

Piece::Piece(const Piece& other, bool copySprite, bool copySquares, bool copyPromotionTypes) : sprite(copySprite ? std::make_unique<sf::Sprite>(*other.sprite) : nullptr), position(other.position), color(other.color), id(other.id), hasMoved(other.hasMoved),
	inCheck(other.inCheck), value(other.value), canMove(other.canMove), promoted(other.promoted), promotedPieceType(other.promotedPieceType), demotedID(other.demotedID)
{
	if (copySquares) {
		availablePositions = other.availablePositions;
		availableCapturePositions = other.availableCapturePositions;
		enPassantPositions = other.enPassantPositions;
		castlePositions = other.castlePositions;
		captureCastlePositions = other.captureCastlePositions;
	}
	positionVectors.push_back(&this->availablePositions);
	positionVectors.push_back(&this->availableCapturePositions);
	positionVectors.push_back(&this->enPassantPositions);
	positionVectors.push_back(&this->castlePositions);
	positionVectors.push_back(&this->captureCastlePositions);
}

bool Piece::operator==(const Piece& other) const
{
	return id == other.id && color == other.color && position == other.position;
}

std::shared_ptr<Piece> Piece::clone(bool copySprite, bool copySquares, bool copyPromotionTypes) const
{
	return std::make_shared<Piece>(*this, copySprite, copySquares, copyPromotionTypes);
}

Piece::~Piece()
{
}

void Piece::makeSprite(const sf::Texture& texture, float scale, sf::Vector2f boardOffset, sf::Vector2f boardSize, sf::Vector2u boardSquares, float boardMultiplier, bool reversed)
{
	if (!sprite) {
		sprite = std::make_unique<sf::Sprite>(texture);
		sprite->setPosition(Chess::getGlobalPosition(position, boardOffset, boardSize, boardMultiplier, boardSquares, reversed));
		sprite->setOrigin(sprite->getLocalBounds().getCenter());
		sprite->setScale(sf::Vector2f(scale, scale));
	}
}

void Piece::setTexture(const sf::Texture& texture, float pieceScale) {
	if (sprite) {
		sprite->setTexture(texture, true);
		sprite->setOrigin(sprite->getLocalBounds().getCenter());
		sprite->setScale(sf::Vector2f{ pieceScale, pieceScale });
	}
}

void Piece::setLocalPosition(sf::Vector2i pos)
{
	position = { pos };
}

void Piece::reversePosition(sf::Vector2f boardOffset, sf::Vector2f boardSize)
{
	setPosition(Chess::reversePosition(getGlobalPos(), boardSize) + boardOffset + boardOffset);
	if (targetPos.has_value()) {
		targetPos = Chess::reversePosition(targetPos.value(), boardSize) + boardOffset + boardOffset;
	}
	if (animationTarget.has_value()) {
		animationTarget = Chess::reversePosition(animationTarget.value(), boardSize) + boardOffset + boardOffset;
	}
}

void Piece::setPosition(sf::Vector2f pos)
{
	if (sprite) {
		sprite->setPosition(pos);
	}
}

void Piece::setPosition(sf::Vector2i pos)
{
	if (sprite) {
		sprite->setPosition((sf::Vector2f)pos);
	}
}

void Piece::addMoveSquare(sf::Vector2i square)
{
	std::string m = Chess::convertPositiontoNotation(position) + Chess::convertPositiontoNotation(square);
	availablePositions.emplace_back(square, m);
}

void Piece::addCaptureSquare(sf::Vector2i square)
{
	std::string m = Chess::convertPositiontoNotation(position) + Chess::convertPositiontoNotation(square);
	availableCapturePositions.emplace_back(square, m);
}

void Piece::addEnPassantSquare(sf::Vector2i square)
{
	std::string m = Chess::convertPositiontoNotation(position) + Chess::convertPositiontoNotation(square);
	enPassantPositions.emplace_back(square, m);
}

void Piece::setVisible(bool visible)
{
	if (sprite) {
		setSpriteVisible(*sprite, visible);
	}
}

void Piece::generateLegalMoves(const Chess::Position& pos, const Chess::Variant& variant)
{
	canMove = false;
	availablePositions.clear();
	availableCapturePositions.clear();
	enPassantPositions.clear();
	castlePositions.clear();
	captureCastlePositions.clear();
	if (variant.pieceMoves.find(id) != variant.pieceMoves.end()) {
		const Chess::MoveSetVector& moveSets = variant.pieceMoves.find(id)->second;
		Chess::SquareSet moves, captures;
		for (const auto& set : moveSets) {
			bool moved = variant.sRanks.at(position.y - 1).at(position.x - 1) != (isWhite() ? std::toupper(id) : id);
			if (!moved || !set->initial) {
				auto pieceMoves = set->getSquares(pos.pieceList, color, position, variant.boardSize, variant.janggiCannons.has(id));
				moves.merge(pieceMoves.moveSet);
				captures.merge(pieceMoves.captureSet);
			}
		}
		for (auto& move : moves) {
			addMove(move, pos, variant);
		}
		for (auto& capture : captures) {
			addCapture(capture, pos, variant);
			// Only for royal pieces check
			Chess::getPieceFromPosition(capture, pos.pieceList)->addAttacker(variant, pos);
		}
	}
	if (variant.pawnDoubleStep && variant.pawnPiecesBlastImmunity.has(id)) {
		if (((color == Chess::PColor::White && variant.whiteDoubleStepRegion.contains(position))
			|| (color == Chess::PColor::Black && variant.blackDoubleStepRegion.contains(position)))) {
			sf::Vector2i singleStep = { position.x, position.y + (color == Chess::PColor::White ? 1 : -1) };
			sf::Vector2i doubleStep = { position.x, position.y + (color == Chess::PColor::White ? 2 : -2) };
			if (Chess::isValidSquare(doubleStep, variant.boardSize) && Chess::getPieceFromPosition(singleStep, pos.pieceList) == nullptr &&
				Chess::getPieceFromPosition(doubleStep, pos.pieceList) == nullptr) {
				addMove(doubleStep, pos, variant);
			}
		}
		if (((color == Chess::PColor::White && variant.whiteTripleStepRegion.contains(position))
			|| (color == Chess::PColor::Black && variant.blackTripleStepRegion.contains(position)))) {
			sf::Vector2i singleStep = { position.x, position.y + (color == Chess::PColor::White ? 1 : -1) };
			sf::Vector2i doubleStep = { position.x, position.y + (color == Chess::PColor::White ? 2 : -2) };
			sf::Vector2i tripleStep = { position.x, position.y + (color == Chess::PColor::White ? 3 : -3) };
			if (Chess::isValidSquare(tripleStep, variant.boardSize) && Chess::getPieceFromPosition(singleStep, pos.pieceList) == nullptr &&
				Chess::getPieceFromPosition(doubleStep, pos.pieceList) == nullptr && Chess::getPieceFromPosition(tripleStep, pos.pieceList) == nullptr) {
				addMove(tripleStep, pos, variant);
			}
		}
	}
	if (pos.enPassantTarget.has_value() && (variant.pawnPieces.has(id) || variant.enPassantPieces.has(id))) {
		if (pos.enPassantTarget.value() == sf::Vector2i{ position.x + 1, position.y }) {
			std::shared_ptr<Piece> piece = Chess::getPieceFromPosition(pos.enPassantTarget.value(), pos.pieceList);
			if (piece != nullptr && piece->color != color && (variant.pawnPieces.has(piece->id) || variant.enPassantPieces.has(piece->id))) {
				if (piece->color == Chess::PColor::White) {
					if (isValidPassant({ piece->getLocalPos().x, piece->getLocalPos().y - 1 }, pos, variant)) {
						addEnPassantSquare(sf::Vector2i{ piece->getLocalPos().x, piece->getLocalPos().y - 1 });
					}
				}
				else {
					if (isValidPassant({ piece->getLocalPos().x, piece->getLocalPos().y + 1 }, pos, variant)) {
						addEnPassantSquare(sf::Vector2i{ piece->getLocalPos().x, piece->getLocalPos().y + 1 });
					}
				}
			}
		}
		if (pos.enPassantTarget.value() == sf::Vector2i{ position.x - 1, position.y }) {
			std::shared_ptr<Piece> piece = Chess::getPieceFromPosition(pos.enPassantTarget.value(), pos.pieceList);
			if (piece != nullptr && piece->color != color && (variant.pawnPieces.has(piece->id) || variant.enPassantPieces.has(piece->id))) {
				if (piece->color == Chess::PColor::White) {
					if (isValidPassant({ piece->getLocalPos().x, piece->getLocalPos().y - 1 }, pos, variant)) {
						addEnPassantSquare(sf::Vector2i{ piece->getLocalPos().x, piece->getLocalPos().y - 1 });
					}
				}
				else {
					if (isValidPassant({ piece->getLocalPos().x, piece->getLocalPos().y + 1 }, pos, variant)) {
						addEnPassantSquare(sf::Vector2i{ piece->getLocalPos().x, piece->getLocalPos().y + 1 });
					}
				}
			}
		}
		if (pos.enPassantTripleTarget.has_value()) {
			int offset = isWhite() ? -1 : 1;
			if (pos.enPassantTripleTarget.value() == sf::Vector2i{ position.x + 1, position.y + offset }) {
				std::shared_ptr<Piece> piece = Chess::getPieceFromPosition(pos.enPassantTripleTarget.value(), pos.pieceList);
				if (piece && piece->color != color && (variant.pawnPieces.has(piece->id) || variant.enPassantPieces.has(piece->id))) {
					if (piece->color == Chess::PColor::White) {
						if (isValidPassant({ piece->getLocalPos().x, piece->getLocalPos().y - 2 }, pos, variant)) {
							addEnPassantSquare(sf::Vector2i{ piece->getLocalPos().x, piece->getLocalPos().y - 2 });
						}
					}
					else {
						if (isValidPassant({ piece->getLocalPos().x, piece->getLocalPos().y + 2 }, pos, variant)) {
							addEnPassantSquare(sf::Vector2i{ piece->getLocalPos().x, piece->getLocalPos().y + 2 });
						}
					}
				}
			}
			if (pos.enPassantTripleTarget.value() == sf::Vector2i{ position.x - 1, position.y + offset }) {
				std::shared_ptr<Piece> piece = Chess::getPieceFromPosition(pos.enPassantTripleTarget.value(), pos.pieceList);
				if (piece && piece->color != color && (variant.pawnPieces.has(piece->id) || variant.enPassantPieces.has(piece->id))) {
					if (piece->color == Chess::PColor::White) {
						if (isValidPassant({ piece->getLocalPos().x, piece->getLocalPos().y - 2 }, pos, variant)) {
							addEnPassantSquare(sf::Vector2i{ piece->getLocalPos().x, piece->getLocalPos().y - 2 });
						}
					}
					else {
						if (isValidPassant({ piece->getLocalPos().x, piece->getLocalPos().y + 2 }, pos, variant)) {
							addEnPassantSquare(sf::Vector2i{ piece->getLocalPos().x, piece->getLocalPos().y + 2 });
						}
					}
				}
			}
		}
	}
	if (variant.castlingEnabled && variant.castlingKingPiece.has(id)) {
		int Krook = (color == Chess::PColor::White) ? pos.castlePieces.at(0) : pos.castlePieces.at(2);
		int Qrook = (color == Chess::PColor::White) ? pos.castlePieces.at(1) : pos.castlePieces.at(3);
		std::pair<bool, bool> castlingRights = calculateCastlingRights(pos, variant);
		if (castlingRights.first) {
			sf::Vector2i capturePos = { Krook, position.y };
			addCastleCaptureSquare(capturePos, pos.hasChess960, true, variant);
			if (!pos.hasChess960) {
				sf::Vector2i castlePos = sf::Vector2i{ variant.castleKDestination, position.y };
				addCastleSquare(castlePos);
			}
		}
		if (castlingRights.second) {
			sf::Vector2i capturePos = { Qrook, position.y };
			addCastleCaptureSquare(capturePos, pos.hasChess960, false, variant);
			if (!pos.hasChess960) {
				sf::Vector2i castlePos = sf::Vector2i{ variant.castleQDestination, position.y };
				addCastleSquare(castlePos);
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
	if (!castlePositions.empty()) {
		canMove = true;
	}
	if (!captureCastlePositions.empty()) {
		canMove = true;
	}
}

bool Piece::validatePosition(const Chess::Position& pos, const Chess::Variant& variant) const
{
	if (variant.pieceMoves.find(id) != variant.pieceMoves.end()) {
		for (const auto& set : variant.pieceMoves.find(id)->second) {
			bool moved = variant.sRanks.at(position.y - 1).at(position.x - 1) != (isWhite() ? std::toupper(id) : id);
			if (!moved || !set->initial) {
				auto moves = set->getSquares(pos.pieceList, color, position, variant.boardSize, variant.janggiCannons.has(id));
				for (auto& capture : moves.captureSet) {
					auto piece = Chess::getPieceFromPosition(capture, pos.pieceList);
					if (piece && Chess::isRoyal(piece, variant, pos.extinctionRoyalPieces) && piece->color != color) {
						return false;
					}
				}
			}
		}
	}
	return true;
}

void Piece::updateScale(float scale, sf::Vector2f boardOffset, sf::Vector2f boardSize, sf::Vector2u boardSquares, float boardMultiplier, float reversed)
{
	if (sprite) {
		sprite->setScale(sf::Vector2f(scale, scale));
		sprite->setPosition(Chess::getGlobalPosition(position, boardOffset, boardSize, boardMultiplier, boardSquares, reversed));
	}
}

std::pair<bool, bool> Piece::calculateCastlingRights(const Chess::Position& pos, const Chess::Variant& variant)
{
	bool canCastleK = false;
	bool canCastleQ = false;
	int Krook = (color == Chess::PColor::White) ? pos.castlePieces.at(0) : pos.castlePieces.at(2);
	int Qrook = (color == Chess::PColor::White) ? pos.castlePieces.at(1) : pos.castlePieces.at(3);
	bool canNeverCastleK = (color == Chess::PColor::White) ? !pos.castlingRights.at(0) : !pos.castlingRights.at(2);
	bool canNeverCastleQ = (color == Chess::PColor::White) ? !pos.castlingRights.at(1) : !pos.castlingRights.at(3);
	int castleRank = (color == Chess::PColor::White) ? variant.castleRank : Chess::reverseY(variant.castleRank, variant.boardSize);
	int startKRook = (color == Chess::PColor::White) ? variant.startKRookW : variant.startKRookB;
	int startQRook = (color == Chess::PColor::White) ? variant.startQRookW : variant.startQRookB;
	if (!hasMoved && !inCheck && position.y == castleRank) {
		// Kingside
		if (!canNeverCastleK && Krook != -1) {
			if (!pos.hasChess960 && Krook != startKRook) { goto Queenside; }
			if (Chess::getPieceFromPosition({ Krook, position.y }, pos.pieceList)) {
				std::shared_ptr<Piece> rook = Chess::getPieceFromPosition({ Krook, position.y }, pos.pieceList);
				if (variant.castlingRookPieces.has(rook->id) && rook->color == color && !rook->hasMoved) {
					std::vector<sf::Vector2i> castleSquares, kingSquares;
					sf::Vector2i target = { variant.castleKDestination, position.y };
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
					auto find = std::find(castleSquares.begin(), castleSquares.end(), sf::Vector2i{ variant.castleKDestination, position.y });
					if (find == castleSquares.end()) {
						castleSquares.push_back(sf::Vector2i{ variant.castleKDestination, position.y });
					}
					auto find2 = std::find(castleSquares.begin(), castleSquares.end(), sf::Vector2i{ variant.castleKDestination - 1, position.y });
					if (find2 == castleSquares.end()) {
						castleSquares.push_back(sf::Vector2i{ variant.castleKDestination - 1, position.y });
					}
					for (const auto& sq : castleSquares) {
						if (Chess::getPieceFromPosition(sq, pos.pieceList)) {
							if (sq != rook->getLocalPos() && sq != getLocalPos()) {
								goto Queenside;
							}
						}
					}
					for (const auto& sq : kingSquares) {
						if (Chess::getPieceFromPosition(sq, pos.pieceList)) {
							if (sq != rook->getLocalPos() && sq != getLocalPos()) {
								goto Queenside;
							}
						}
						Chess::Position newPos = pos;
						newPos.pieceList = Chess::makePieceVec(pos.pieceList, sq, getLocalPos());
						newPos.extinctionRoyalPieces = Chess::getExtinctionRoyalPieces(variant, newPos.pieceList);
						if (!Chess::isValidPosition(newPos, variant, color)) {
							goto Queenside;
						}
					}
					canCastleK = true;
				}
			}
		}
	Queenside:
		if (!canNeverCastleQ && Qrook != -1) {
			if (!pos.hasChess960 && Qrook != startQRook) { goto nextCheck; }
			if (Chess::getPieceFromPosition({ Qrook, position.y }, pos.pieceList)) {
				std::shared_ptr<Piece> rook = Chess::getPieceFromPosition({ Qrook, position.y }, pos.pieceList);
				if (variant.castlingRookPieces.has(rook->id) && rook->color == color && !rook->hasMoved) {
					std::vector<sf::Vector2i> castleSquares, kingSquares;
					sf::Vector2i target = { variant.castleQDestination, position.y };
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
					auto find = std::find(castleSquares.begin(), castleSquares.end(), sf::Vector2i{ variant.castleQDestination, position.y });
					if (find == castleSquares.end()) {
						castleSquares.push_back(sf::Vector2i{ variant.castleQDestination, position.y });
					}
					auto find2 = std::find(castleSquares.begin(), castleSquares.end(), sf::Vector2i{ variant.castleQDestination + 1, position.y });
					if (find2 == castleSquares.end()) {
						castleSquares.push_back(sf::Vector2i{ variant.castleQDestination + 1, position.y });
					}
					for (const auto& sq : castleSquares) {
						if (Chess::getPieceFromPosition(sq, pos.pieceList)) {
							if (sq != rook->getLocalPos() && sq != getLocalPos()) {
								goto nextCheck;
							}
						}
					}
					for (const auto& sq : kingSquares) {
						if (Chess::getPieceFromPosition(sq, pos.pieceList)) {
							if (sq != rook->getLocalPos() && sq != getLocalPos()) {
								goto nextCheck;
							}
						}
						Chess::Position newPos = pos;
						newPos.pieceList = Chess::makePieceVec(pos.pieceList, sq, getLocalPos());
						newPos.extinctionRoyalPieces = Chess::getExtinctionRoyalPieces(variant, newPos.pieceList);
						if (!Chess::isValidPosition(newPos, variant, color)) {
							goto nextCheck;
						}
					}
					canCastleQ = true;
				}
			}
		}
	}
nextCheck:
	if (canCastleK) {
		sf::Vector2i capturePos = { Krook, position.y };
		pieceVector newV;
		newV.resize(pos.pieceList.size());
		for (size_t j = 0; j < pos.pieceList.size(); j++)
		{
			newV[j] = pos.pieceList[j]->clone(false, false);
		}
		for (auto& v : newV) {
			// Rook
			if (v->getLocalPos() == sf::Vector2i{ Krook, position.y }) {
				v->setLocalPosition(sf::Vector2i{ variant.castleKDestination - 1, position.y });
				v->hasMoved = true;
			}
			// King
			if (v->getLocalPos() == getLocalPos()) {
				v->setLocalPosition(sf::Vector2i{ variant.castleKDestination, position.y });
				v->hasMoved = true;
			}
		}
		Chess::Position newPos = pos;
		newPos.pieceList = newV;
		newPos.extinctionRoyalPieces = Chess::getExtinctionRoyalPieces(variant, newPos.pieceList);
		if (variant.checksEnabled) {
			if (!isValidMovePos(pos, newPos, variant, { variant.castleKDestination, position.y }, color)) {
				canCastleK = false;
			}
			else { canMove = true; }
		}
	}
	if (canCastleQ) {
		sf::Vector2i capturePos = { Qrook, position.y };
		pieceVector newV;
		newV.resize(pos.pieceList.size());
		for (size_t j = 0; j < pos.pieceList.size(); j++)
		{
			newV[j] = pos.pieceList[j]->clone(false, false);
		}
		for (auto& v : newV) {
			// Rook
			if (v->getLocalPos() == sf::Vector2i{ Qrook, position.y }) {
				v->setLocalPosition(sf::Vector2i{ variant.castleQDestination + 1, position.y });
				v->hasMoved = true;
			}
			// King
			if (v->getLocalPos() == getLocalPos()) {
				v->setLocalPosition(sf::Vector2i{ variant.castleQDestination, position.y });
				v->hasMoved = true;
			}
		}
		if (variant.checksEnabled) {
			Chess::Position newPos = pos;
			newPos.pieceList = newV;
			newPos.extinctionRoyalPieces = Chess::getExtinctionRoyalPieces(variant, newPos.pieceList);
			if (!isValidMovePos(pos, newPos, variant, { variant.castleQDestination, position.y }, color)) {
				canCastleQ = false;
			}
			else { canMove = true; }
		}
		newV.clear();
	}
	return std::make_pair(canCastleK, canCastleQ);
}

void Piece::addCastleSquare(sf::Vector2i square)
{
	auto& moves = getMoveSquares();
	bool find = std::find_if(moves.begin(), moves.end(), [square](Chess::Square& sq) { return sq.pos == square; }) != moves.end();
	if (!find) {
		std::string m = Chess::convertPositiontoNotation(position) + Chess::convertPositiontoNotation(square);
		castlePositions.emplace_back(square, m);
	}
}

void Piece::addCastleCaptureSquare(sf::Vector2i square, bool chess960, bool kingside, const Chess::Variant& variant)
{
	std::string m;
	auto& moves = getMoveSquares();
	auto& captureMoves = getCaptureSquares();
	int dest = kingside ? variant.castleKDestination : variant.castleQDestination;
	bool find = std::find_if(moves.begin(), moves.end(), [&](Chess::Square& sq) { return sq.pos == sf::Vector2i{ dest, position.y }; }) != moves.end() ||
		std::find_if(captureMoves.begin(), captureMoves.end(), [&](Chess::Square& sq) { return sq.pos == sf::Vector2i{ dest, position.y }; }) != captureMoves.end();
	if (chess960 || find) {
		m = Chess::convertPositiontoNotation(position) + Chess::convertPositiontoNotation(square);
	}
	else {
		m = Chess::convertPositiontoNotation(position) + Chess::convertPositiontoNotation({ dest, position.y });
	}
	captureCastlePositions.emplace_back(square, m);
}

bool Piece::addMove(sf::Vector2i square, const Chess::Position& pos, const Chess::Variant& variant)
{
	Chess::PieceType validPromotionTypes;
	const auto& currentPromotionRegion = isWhite() ? variant.whitePromotionRegion : variant.blackPromotionRegion;
	const auto& currentPromotedPieces = isWhite() ? variant.whitePromotePieces : variant.blackPromotePieces;
	bool notLastRank = ((isWhite() && square.y != variant.boardSize.y) || (isBlack() && square.y != 1));

	Chess::Position newPos = pos;
	bool isPawnPromotion = variant.promotionPawnPieces.has(id) || variant.pawnPieces.has(id);
	bool isPiecePromotion = (variant.promotedPieceTypes.find(id) != variant.promotedPieceTypes.end()) || (variant.pieceDemotion && promotedPieceType && demotedID != ' ');
	if ((isPawnPromotion || isPiecePromotion) && currentPromotionRegion.contains(square)) {
		if (isPiecePromotion && isPawnPromotion) {
			validPromotionTypes = currentPromotedPieces;
			if ((variant.promotedPieceTypes.find(id) != variant.promotedPieceTypes.end())) {
				validPromotionTypes.add(variant.promotedPieceTypes.find(id)->second);
			}
			if (!variant.forcePiecePromotion && !variant.forcePawnPromotion) {
				validPromotionTypes.add(id);
			}
			if (variant.pieceDemotion && promotedPieceType && demotedID != ' ') {
				validPromotionTypes.add(demotedID);
			}
		}
		else if (isPawnPromotion) {
			validPromotionTypes = currentPromotedPieces;
			if (!variant.forcePawnPromotion && notLastRank) { validPromotionTypes.add(id); }
		}
		else if (isPiecePromotion) {
			if ((variant.promotedPieceTypes.find(id) != variant.promotedPieceTypes.end())) {
				validPromotionTypes.add(variant.promotedPieceTypes.find(id)->second);
			}
			if (!variant.forcePiecePromotion) { validPromotionTypes.add(id); }
			if (variant.pieceDemotion && promotedPieceType && demotedID != ' ') {
				validPromotionTypes.add(demotedID);
			}
		}
		validatePromotionLimit(validPromotionTypes, variant, pos);
		pieceVector newV;
		std::shared_ptr<Piece> promotedPiece = nullptr;
		for (const auto& piece : pos.pieceList)
		{
			newV.push_back(piece->clone(false, false));
		}
		for (auto& v : newV) {
			if (v->getLocalPos() == getLocalPos()) {
				v->setLocalPosition(square);
				v->hasMoved = true;
				promotedPiece = v;
				break;
			}
		}
		if (promotedPiece) {
			for (auto& p : validPromotionTypes) {
				promotedPiece->id = p;
				newPos.pieceList = newV;
				newPos.extinctionRoyalPieces = Chess::getExtinctionRoyalPieces(variant, newPos.pieceList);
				if (!isValidMovePos(pos, newPos, variant, square, color)) {
					validPromotionTypes.remove(p);
				}
			}
		}
		if (validPromotionTypes.empty()) {
			return false;
		}
		else {
			std::string m = Chess::convertPositiontoNotation(position) + Chess::convertPositiontoNotation(square);
			availablePositions.emplace_back(square, m, validPromotionTypes, true);
			return true;
		}
	}
	else if (isPiecePromotion && currentPromotionRegion.contains(position)) {
		if ((variant.promotedPieceTypes.find(id) != variant.promotedPieceTypes.end())) {
			validPromotionTypes.add(variant.promotedPieceTypes.find(id)->second);
		}
		if (!variant.forcePiecePromotion) { validPromotionTypes.add(id); }
		if (variant.pieceDemotion && promotedPieceType && demotedID != ' ') {
			validPromotionTypes.add(demotedID);
		}
		validatePromotionLimit(validPromotionTypes, variant, pos);
		pieceVector newV;
		std::shared_ptr<Piece> promotedPiece = nullptr;
		for (const auto& piece : pos.pieceList)
		{
			newV.push_back(piece->clone(false, false));
		}
		for (auto& v : newV) {
			if (v->getLocalPos() == getLocalPos()) {
				v->setLocalPosition(square);
				v->hasMoved = true;
				promotedPiece = v;
				break;
			}	
		}
		if (promotedPiece) {
			for (auto& p : validPromotionTypes) {
				promotedPiece->id = p;
				newPos.pieceList = newV;
				newPos.extinctionRoyalPieces = Chess::getExtinctionRoyalPieces(variant, newPos.pieceList);
				if (!isValidMovePos(pos, newPos, variant, square, color)) {
					validPromotionTypes.remove(p);
				}
			}
		}
		if (validPromotionTypes.empty()) {
			return false;
		}
		else {
			std::string m = Chess::convertPositiontoNotation(position) + Chess::convertPositiontoNotation(square);
			availablePositions.emplace_back(square, m, validPromotionTypes, true);
			return true;
		}
	}
	newPos.pieceList = Chess::makePieceVec(pos.pieceList, square, *this, variant);
	newPos.extinctionRoyalPieces = Chess::getExtinctionRoyalPieces(variant, newPos.pieceList);
	if (!isValidMovePos(pos, newPos, variant, square, color)) {
		return false;
	}

	std::string m = Chess::convertPositiontoNotation(position) + Chess::convertPositiontoNotation(square);
	availablePositions.emplace_back(square, m);
	return false;
}

bool Piece::isValidMovePos(const Chess::Position& pos, const Chess::Position& newPos, const Chess::Variant& variant, sf::Vector2i square, Chess::PColor color) const {
	if (variant.atomicExplosions && pos.atomicKings && !Chess::isRoyal(this, variant, pos.extinctionRoyalPieces)) { return true; }
	else if (variant.atomicExplosions && Chess::isRoyal(this, variant, pos.extinctionRoyalPieces)) {
		if (variant.blockChecks) {
			if ((!isValidPosition(newPos, variant, color) || !isValidPosition(pos, variant, !color)) && !enemyKingOccupies(square, pos, variant, color)) {
				return false;
			}
		}
		else if (!isValidPosition(newPos, variant, color) && !enemyKingOccupies(square, pos, variant, color)) {
			return false;
		}
	}
	else {
		if (variant.blockChecks) {
			if (!isValidPosition(newPos, variant, color) || !isValidPosition(newPos, variant, !color)) {
				return false;
			}
		}
		else {
			if (!isValidPosition(newPos, variant, color)) {
				return false;
			}
		}
	}
	if (variant.immobilityIllegal) {
		auto piece = Chess::getPieceFromPosition(square, newPos.pieceList);
		if (piece && variant.pieceMoves.find(piece->id) != variant.pieceMoves.end()) {
			const Chess::MoveSetVector& moveSets = variant.pieceMoves.find(piece->id)->second;
			Chess::SquareSet moves;
			for (const auto& set : moveSets) {
				bool moved = variant.sRanks.at(position.y - 1).at(position.x - 1) != (isWhite() ? std::toupper(id) : id);
				if (!moved || !set->initial) {
					auto pieceMoves = set->getSquares({}, piece->color, piece->getLocalPos(), variant.boardSize, variant.janggiCannons.has(piece->id));
					moves.merge(pieceMoves.moveSet);
				}
			}
			if (moves.empty()) { return false; }
		}
	}
	if (variant.flyingGeneral && !Chess::isValidFlyingGeneral(newPos, variant, color)) {
		return false;
	}
	return true;
}

bool Piece::addCapture(sf::Vector2i square, const Chess::Position& pos, const Chess::Variant& variant)
{
	Chess::PieceType validPromotionTypes;
	const auto& currentPromotionRegion = isWhite() ? variant.whitePromotionRegion : variant.blackPromotionRegion;
	const auto& currentPromotedPieces = isWhite() ? variant.whitePromotePieces : variant.blackPromotePieces;
	bool notLastRank = ((isWhite() && square.y != variant.boardSize.y) || (isBlack() && square.y != 1));

	pieceVector newV = Chess::makePieceVec(pos.pieceList, square, *this, variant);
	Chess::Position newPos = pos;
	newPos.pieceList = newV;
	newPos.extinctionRoyalPieces = Chess::getExtinctionRoyalPieces(variant, newPos.pieceList);
	bool isPawnPromotion = variant.promotionPawnPieces.has(id) || variant.pawnPieces.has(id);
	bool isPiecePromotion = (variant.promotedPieceTypes.find(id) != variant.promotedPieceTypes.end()) || (variant.pieceDemotion && promotedPieceType && demotedID != ' ');
	if ((isPawnPromotion || isPiecePromotion) && currentPromotionRegion.contains(square)) {
		if (isPiecePromotion && isPawnPromotion) {
			validPromotionTypes = currentPromotedPieces;
			if ((variant.promotedPieceTypes.find(id) != variant.promotedPieceTypes.end())) {
				validPromotionTypes.add(variant.promotedPieceTypes.find(id)->second);
			}
			if (!variant.forcePiecePromotion && !variant.forcePawnPromotion) {
				validPromotionTypes.add(id);
			}
			if (variant.pieceDemotion && promotedPieceType && demotedID != ' ') {
				validPromotionTypes.add(demotedID);
			}
		}
		else if (isPawnPromotion) {
			validPromotionTypes = currentPromotedPieces;
			if (!variant.forcePawnPromotion && notLastRank) { validPromotionTypes.add(id); }
		}
		else if (isPiecePromotion) {
			if ((variant.promotedPieceTypes.find(id) != variant.promotedPieceTypes.end())) {
				validPromotionTypes.add(variant.promotedPieceTypes.find(id)->second);
			}
			if (!variant.forcePiecePromotion) { validPromotionTypes.add(id); }
			if (variant.pieceDemotion && promotedPieceType && demotedID != ' ') {
				validPromotionTypes.add(demotedID);
			}
		}
		validatePromotionLimit(validPromotionTypes, variant, pos);
		if (!variant.atomicExplosions || !variant.atomicImmunePieces.has(id)) {
			std::shared_ptr<Piece> promotedPiece = nullptr;
			for (auto& v : newV) {
				if (v->getLocalPos() == square) {
					promotedPiece = v;
					break;
				}
			}
			if (promotedPiece) {
				for (auto& p : validPromotionTypes) {
					promotedPiece->id = p;
					newPos.pieceList = newV;
					newPos.extinctionRoyalPieces = Chess::getExtinctionRoyalPieces(variant, newPos.pieceList);
					if (!isValidCapturePos(pos, newPos, variant, square, color)) {
						validPromotionTypes.remove(p);
					}
				}
			}
		}
		if (validPromotionTypes.empty()) {
			return false;
		}
		else {
			std::string m = Chess::convertPositiontoNotation(position) + Chess::convertPositiontoNotation(square);
			availableCapturePositions.emplace_back(square, m, validPromotionTypes, true);
			return true;
		}
	}
	else if (isPiecePromotion && currentPromotionRegion.contains(position)) {
		if ((variant.promotedPieceTypes.find(id) != variant.promotedPieceTypes.end())) {
			validPromotionTypes.add(variant.promotedPieceTypes.find(id)->second);
		}
		if (!variant.forcePiecePromotion) { validPromotionTypes.add(id); }
		if (variant.pieceDemotion && promotedPieceType && demotedID != ' ') {
			validPromotionTypes.add(demotedID);
		}
		validatePromotionLimit(validPromotionTypes, variant, pos);
		if (!variant.atomicExplosions || !variant.atomicImmunePieces.has(id)) {
			std::shared_ptr<Piece> promotedPiece = nullptr;
			for (auto& v : newV) {
				if (v->getLocalPos() == square) {
					promotedPiece = v;
					break;
				}
			}
			if (promotedPiece) {
				for (auto& p : validPromotionTypes) {
					promotedPiece->id = p;
					newPos.pieceList = newV;
					newPos.extinctionRoyalPieces = Chess::getExtinctionRoyalPieces(variant, newPos.pieceList);
					if (!isValidCapturePos(pos, newPos, variant, square, color)) {
						validPromotionTypes.remove(p);
					}
				}
			}
		}
		if (validPromotionTypes.empty()) {
			return false;
		}
		else {
			std::string m = Chess::convertPositiontoNotation(position) + Chess::convertPositiontoNotation(square);
			availableCapturePositions.emplace_back(square, m, validPromotionTypes, true);
			return true;
		}
	}
	if (!isValidCapturePos(pos, newPos, variant, square, color)) { return false; }

	std::string m = Chess::convertPositiontoNotation(position) + Chess::convertPositiontoNotation(square);
	availableCapturePositions.emplace_back(square, m);
	return true;
}

bool Piece::isValidCapturePos(const Chess::Position& pos, const Chess::Position& newPos, const Chess::Variant& variant, sf::Vector2i square, Chess::PColor color) const {
	if (!pos.atomicKings) {
		if (variant.atomicExplosions) {
			if (!isValidPosition(newPos, variant, color)) {
				if (!canCaptureEnemyKing(square, pos, variant, color) || !isValidAtomicCapture(square, pos, variant, color)) {
					return false;
				}
			}
			else if (!isValidAtomicCapture(square, pos, variant, color)) {
				return false;
			}
		}
		else if (variant.blockChecks) {
			if (!isValidPosition(newPos, variant, color) || !isValidPosition(newPos, variant, !color)) {
				return false;
			}
		}
		else {
			auto piece = Chess::getPieceFromPosition(square, pos.pieceList);
			if (!isValidPosition(newPos, variant, color) && !(Chess::isRoyal(piece, variant, pos.extinctionRoyalPieces) && piece->color != color)) {
				return false;
			}
		}
	}
	else {
		if (!isValidAtomicCapture(square, pos, variant, color)) {
			return false;
		}
	}
	if (variant.immobilityIllegal) {
		auto piece = Chess::getPieceFromPosition(square, newPos.pieceList);
		if (piece && variant.pieceMoves.find(piece->id) != variant.pieceMoves.end()) {
			const Chess::MoveSetVector& moveSets = variant.pieceMoves.find(piece->id)->second;
			Chess::SquareSet moves;
			for (const auto& set : moveSets) {
				bool moved = variant.sRanks.at(position.y - 1).at(position.x - 1) != (isWhite() ? std::toupper(id) : id);
				if (!moved || !set->initial) {
					auto pieceMoves = set->getSquares({}, piece->color, piece->getLocalPos(), variant.boardSize, variant.janggiCannons.has(piece->id));
					moves.merge(pieceMoves.moveSet);
				}
			}
			if (moves.empty()) { return false; }
		}
	}
	if (variant.flyingGeneral && !Chess::isValidFlyingGeneral(newPos, variant, color)) {
		return false;
	}
	return true;
}

void Piece::validatePromotionLimit(Chess::PieceType& validTypes, const Chess::Variant& variant, const Chess::Position& pos) const
{
	if (!variant.promotionLimit.empty()) {
		for (auto& type : validTypes) {
			if (type != id) {
				auto find = variant.promotionLimit.find(type);
				if (find != variant.promotionLimit.end()) {
					bool w = isWhite();
					int count = std::count_if(pos.pieceList.begin(), pos.pieceList.end(),
						[type, this](const std::shared_ptr<Piece>& piece) { return piece->color == color && piece->id == type; });
					if (count >= find->second) {
						validTypes.remove(type);
					}
				}
			}
		}
	}
}

bool Piece::isValidPassant(sf::Vector2i square, const Chess::Position& pos, const Chess::Variant& variant) const
{
	if (variant.forcePawnPromotion && (variant.promotionPawnPieces.has(id) || variant.pawnPieces.has(id))) {
		const auto& currentPromotionRegion = isWhite() ? variant.whitePromotionRegion : variant.blackPromotionRegion;
		const auto& currentPromotePieces = isWhite() ? variant.whitePromotePieces : variant.blackPromotePieces;
		if (currentPromotionRegion.contains(square) && currentPromotePieces.empty()) { return false; }
	}
	pieceVector newV{};
	newV.resize(pos.pieceList.size());
	for (size_t j = 0; j < pos.pieceList.size(); ++j)
	{
		newV[j] = pos.pieceList[j]->clone(false, false);
	}
	sf::Vector2i enPassantPos{ 0, 0 };
	if (color == Chess::PColor::White) {
		enPassantPos = { square.x, square.y - 1 };
	}
	else {
		enPassantPos = { square.x, square.y + 1 };
	}
	bool find = false;
	for (int j = 0; j < newV.size(); j++) {
		if (newV.at(j)->getLocalPos() == enPassantPos) {
			newV.erase(newV.begin() + j);
			find = true;
			break;
		}
	}
	if (!find) {
		if (color == Chess::PColor::White) {
			enPassantPos = { square.x, square.y - 2 };
		}
		else {
			enPassantPos = { square.x, square.y + 2 };
		}
		for (int j = 0; j < newV.size(); j++) {
			if (newV.at(j)->getLocalPos() == enPassantPos) {
				newV.erase(newV.begin() + j);
				break;
			}
		}
	}
	for (auto& v : newV) {
		if (v->getLocalPos() == getLocalPos()) {
			v->setLocalPosition(square);
			v->hasMoved = true;
		}
	}
	Chess::Position newPos = pos;
	newPos.pieceList = newV;
	newPos.extinctionRoyalPieces = Chess::getExtinctionRoyalPieces(variant, newV);
	if (variant.checksEnabled) {
		if (!pos.atomicKings) {
			if (variant.atomicExplosions) {
				std::vector<sf::Vector2i> piecePositions{ square };
				for (int x = -1; x <= 1; x++) {
					for (int y = -1; y <= 1; y++) {
						sf::Vector2i newPos = { square.x + x, square.y + y };
						if (Chess::isValidSquare(newPos, variant.boardSize)) {
							std::shared_ptr<Piece> p = Chess::getPieceFromPosition(newPos, newV);
							if (p != nullptr && !variant.atomicImmunePieces.has(p->id) && !variant.pawnPiecesBlastImmunity.has(p->id)) {
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
				Chess::Position newPos = pos;
				newPos.pieceList = newV;
				newPos.extinctionRoyalPieces = Chess::getExtinctionRoyalPieces(variant, newV);
				if (!Chess::isValidPosition(newPos, variant, color)) {
					if (!(Chess::canCaptureEnemyKing(square, pos, variant, color) && Chess::isValidAtomicCapture(square, pos, variant, color))) {
						return false;
					}
				}
				else if (!Chess::isValidAtomicCapture(square, pos, variant, color)) {
					return false;
				}
			}
			else {
				if (variant.blockChecks) {
					if (!isValidPosition(newPos, variant, color) || !isValidPosition(newPos, variant, !color)) {
						return false;
					}
				}
				else {
					if (!isValidPosition(newPos, variant, color)) {
						return false;
					}
				}
			}
		}
		else {
			if (!Chess::isValidAtomicCapture(square, pos, variant, color)) {
				return false;
			}
		}
	}
	if (variant.immobilityIllegal) {
		auto piece = Chess::getPieceFromPosition(square, newPos.pieceList);
		if (piece && variant.pieceMoves.find(piece->id) != variant.pieceMoves.end()) {
			const Chess::MoveSetVector& moveSets = variant.pieceMoves.find(piece->id)->second;
			Chess::SquareSet moves;
			for (const auto& set : moveSets) {
				bool moved = variant.sRanks.at(position.y - 1).at(position.x - 1) != (isWhite() ? std::toupper(id) : id);
				if (!moved || !set->initial) {
					auto pieceMoves = set->getSquares({}, piece->color, piece->getLocalPos(), variant.boardSize, variant.janggiCannons.has(piece->id));
					moves.merge(pieceMoves.moveSet);
				}
			}
			if (moves.empty()) { return false; }
		}
	}
	if (variant.flyingGeneral && !Chess::isValidFlyingGeneral(newPos, variant, color)) {
		return false;
	}
	
	return true;
}

void Piece::setSprites(const sf::Texture& texture, sf::Vector2f boardOffset, sf::Vector2f boardSize, sf::Vector2u boardSquares, float boardMultiplier, float pieceScale, bool reversed)
{
	for (auto& vec : positionVectors) {
		for (auto& square : *vec) {
			square.setupSprite(texture, boardOffset, boardSize, boardSquares, boardMultiplier, reversed);
		}
	}
}

void Piece::updateSprites(const std::vector<sf::Texture>& boardTextures, sf::Vector2f mousePos, bool& mouseSelecting)
{
	for (auto& square : availablePositions) {
		if (square.sprite != nullptr) {
			auto& sprite = square.sprite;
			if (sprite->getGlobalBounds().contains(mousePos)) {
				mouseSelecting = true;
				sprite->setTexture(boardTextures.at(4), true);
			}
			else { sprite->setTexture(boardTextures.at(0), true); }
		}
	}
	for (auto& square : availableCapturePositions) {
		if (square.sprite != nullptr) {
			auto& sprite = square.sprite;
			if (sprite->getGlobalBounds().contains(mousePos)) {
				mouseSelecting = true;
				sprite->setTexture(boardTextures.at(4));
			}
			else { sprite->setTexture(boardTextures.at(1)); }
		}
	}
	for (auto& square : castlePositions) {
		auto& sprite = square.sprite;
		if (sprite->getGlobalBounds().contains(mousePos)) {
			mouseSelecting = true;
			sprite->setTexture(boardTextures.at(4));
		}
		else { sprite->setTexture(boardTextures.at(0)); }
	}
	for (auto& square : captureCastlePositions) {
		auto& sprite = square.sprite;
		if (sprite->getGlobalBounds().contains(mousePos)) {
			mouseSelecting = true;
			sprite->setTexture(boardTextures.at(4));
		}
		else { sprite->setTexture(boardTextures.at(1)); }
	}
	for (auto& square : enPassantPositions) {
		auto& sprite = square.sprite;
		if (sprite->getGlobalBounds().contains(mousePos)) {
			mouseSelecting = true;
			sprite->setTexture(boardTextures.at(4));
		}
		else { sprite->setTexture(boardTextures.at(0)); }
	}
}

void Piece::drawSprites(sf::RenderWindow& window)
{
	for (auto& vec : positionVectors) {
		for (auto& square : *vec) {
			if (square.sprite) {
				window.draw(*square.sprite);
			}
		}
	}
}

void Piece::updatePosition(float moveSpeed, float deltaTime)
{
	if (sprite) {
		if (animationTarget.has_value()) {
			if (sprite->getPosition() != animationTarget.value()) {
				sprite->setPosition(Interpolate(sprite->getPosition(), animationTarget.value(), 0.22f * deltaTime * 60));
			}
			else {
				animationTarget = {};
			}
		}
		else if (targetPos.has_value()) {
			sprite->setPosition(Interpolate(sprite->getPosition(), targetPos.value(), moveSpeed * 0.25f * deltaTime * 60));
		}
	}
}

void Piece::updatePosition(float moveSpeed, float captureThreshold, float deltaTime)
{
	if (sprite) {
		if (targetPos.has_value()) {
			sprite->setPosition(Interpolate(sprite->getPosition(), targetPos.value(), moveSpeed * 0.25f * deltaTime * 60, captureThreshold));
		}
	}
}

bool Piece::hasMoveIntersecting(sf::Vector2f position) const
{
	for (auto& vec : positionVectors) {
		for (auto& square : *vec) {
			if (square.sprite && square.sprite->getGlobalBounds().contains(position)) {
				return true;
			}
		}
	}
	return false;
}

const Chess::Square& Piece::getMoveIntersecting(sf::Vector2f position) const
{
	for (auto& vec : positionVectors) {
		for (auto& square : *vec) {
			if (square.sprite && square.sprite->getGlobalBounds().contains(position)) {
				return square;
			}
		}
	}
	return positionVectors.front()->front();
}