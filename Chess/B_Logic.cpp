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

#include "Board.h"

// Calculating
void Board::generateLegalMoves(bool init) {
	generatingMoves = true;
	cClock.restart();
	for (auto& piece : whiteDropPieces) {
		piece.update();
	}
	for (auto& piece : blackDropPieces) {
		piece.update();
	}
	position.atomicKings = false;
	if (variant->checksEnabled) {
		position.extinctionRoyalPieces = Chess::getExtinctionRoyalPieces(*variant, position.pieceList);
		if (variant->atomicExplosions) {
			for (int i = 0; i < position.pieceList.size(); i++) {
				if (isRoyal(position.pieceList.at(i))) {
					std::shared_ptr<Piece>& king = position.pieceList.at(i);
					for (int x = -1; x <= 1; x++) {
						for (int y = -1; y <= 1; y++) {
							if (x != 0 || y != 0) {
								sf::Vector2i newPos = { king->getLocalPos().x + x, king->getLocalPos().y + y };
								if (Chess::isValidSquare(newPos, variant->boardSize) && Chess::isPieceAt(newPos, position.pieceList) &&
									isRoyal(getPieceFromCurrentPosition(newPos))) {
									position.atomicKings = true;
									break;
								}
							}
						}
					}
				}
			}
		}
		for (auto& p : position.pieceList) {
			p->inCheck = false;
		}
	}
	for (auto& piece : position.pieceList) {
		piece->generateLegalMoves(position, *variant);
	}
	if (variant->checksEnabled) {
		// Checks
		checkSprites.clear();
		if (!position.atomicKings) {
			for (auto& p : position.pieceList) {
				if (p->inCheck && whiteToPlay == (p->isWhite())) {
					hasCheck = true;
					sf::Sprite checkSprite(boardTextures->at(2));
					checkSprite.setOrigin(checkSprite.getLocalBounds().getCenter());
					checkSprite.setScale(boardSprite.getScale());
					checkSprite.setPosition(getGlobalPosition(p->getLocalPos()));
					checkSprites.push_back(std::move(checkSprite));
					if (!init) {
						if (whiteToPlay) {
							whiteChecksLeft--;
						}
						else {
							blackChecksLeft--;
						}
					}
				}
			}
		}
	}
	if (variant->dropsEnabled) { calculateDropPositions(); }
	if (variant->forceCapture) {
		bool hasCapture = false;
		for (auto& p : position.pieceList) {
			if (p && whiteToPlay == p->isWhite()) {
				if (!p->getCaptureSquares().empty() || !p->getEnPassantSquares().empty()) {
					hasCapture = true;
					break;
				}
			}
		}
		if (hasCapture) {
			for (auto& p : position.pieceList) {
				if (whiteToPlay == p->isWhite()) {
					p->getMoveSquares().clear();
					p->getCastleSquares().clear();
					p->getCaptureCastleSquares().clear();
				}
			}
			auto& currentDropPieces = whiteToPlay ? whiteDropPieces : blackDropPieces;
			for (auto& p : currentDropPieces) {
				p.dropSquares.clear();
			}
		}
	}
	else if (variant->forceDrop) {
		auto& currentDropPieces = whiteToPlay ? whiteDropPieces : blackDropPieces;
		bool hasDrop = std::any_of(currentDropPieces.begin(), currentDropPieces.end(), [](Chess::DropPiece& p) {
			return !p.dropSquares.empty(); });
		if (hasDrop) {
			for (auto& p : position.pieceList) {
				if (whiteToPlay == p->isWhite()) {
					p->getCaptureSquares().clear();
					p->getMoveSquares().clear();
					p->getCastleSquares().clear();
					p->getCaptureCastleSquares().clear();
				}
			}
		}
	}
	std::cout << "Finished Calculating in: " << cClock.reset().asMicroseconds() << "μs." << std::endl;
	cClock.reset();
	setPieceSprites();
	auto pos = savePosition();
	if (allPositionsPlayed.count(pos) > 0) {
		allPositionsPlayed.find(pos)->second++;
	}
	else {
		allPositionsPlayed.emplace(pos, 1);
	}
	if (determineEnd() != Chess::Endgame::None) {
		std::string sideToPlayString = whiteToPlay ? "White " : "Black ";
		position.gameEnded = true;
		switch (determineEnd()) {
		case Chess::Endgame::Checkmate:
		{
			setGameEndText(sideToPlayString + "Lost to Checkmate!");
			break;
		}
		case Chess::Endgame::VariantVictory:
		{
			setGameEndText(sideToPlayString + "Won! Variant Ending.");
			break;
		}
		case Chess::Endgame::VariantLoss:
		{
			setGameEndText(sideToPlayString + "Lost! Variant Ending.");
			break;
		}
		case Chess::Endgame::Draw:
		{
			setGameEndText("Game Ended in a Draw!");
			break;
		}
		case Chess::Endgame::ThreefoldRepetition:
		{
			setGameEndText("Game Ended by Threefold Repetition!");
			break;
		}
		case Chess::Endgame::FiftyMoveRule:
		{
			setGameEndText("Game Ended by 50 Move Rule!");
			break;
		}
		}
	}
	std::vector<sf::Vector2i> checkPositions;
	for (auto& s : checkSprites) {
		checkPositions.push_back(getLocalPosition(s.getPosition()));
	}
	positionHistory.emplace_back(position.pieceList, getCurrentFEN(), moves, position.gameEnded, whiteToPlay, position.enPassantTarget, position.enPassantTripleTarget, position.extraFlagMove, halfMoves, fullMoves, whiteChecksLeft, blackChecksLeft, hasCheck, lastMoveStartLocal, lastMoveDestLocal,
		checkPositions, whiteTime, blackTime, position.castlePieces, position.castlingRights);
	if (!positionHistoryF.empty()) {
		if (positionHistory.back() == positionHistoryF.front()) {
			positionHistoryF.erase(positionHistoryF.begin());
		}
		else {
			positionHistoryF.clear();
		}
	}
	/*
	if (!gameEnded) {
		size_t moveCount = getMoveCount();
		int n = stockfish.getLegalMoveCount(startingFEN, moves);
		if (n != moveCount && n != 0) {
			std::cout << "===========================MISMATCH===========================" << std::endl;
			std::cout << "Count: " << moveCount << std::endl;
			std::cout << "FSF Count: " << n << std::endl;
			std::cout << stockfish.getFEN() << std::endl;
			std::cout << getCurrentFEN() << std::endl;
			auto sfMoves = stockfish.getLegalMoves();
			std::vector<std::string> moves;
			for (auto& piece : position.pieceList) {
				if (whiteToPlay == piece->isWhite()) {
					for (auto& v : piece->getPositionVectors()) {
						for (auto& m : *v) {
							if (std::count(sfMoves.begin(), sfMoves.end(), m.moveString) != 1) {
								std::cout << m.moveString << std::endl;
							}
							else { moves.push_back(m.moveString); }
						}
					}
				}
			}
			std::cout << "//SF MOVES:\n";
			for (auto& move : sfMoves) {
				if (std::count(moves.begin(), moves.end(), move) != 1) {
					std::cout << move << std::endl;
				}
			}
			std::cout << "MOVES SIZE " << moves.size() << "\n";
			isPaused = true;
			stockfishEnabled = false;
		}
	}
	*/
	generatingMoves = false;
}

void Board::mouseModeOff()
{
	mouseMode = false;
}

bool Board::intersects(sf::Vector2f position) const
{
	return boardSprite.getGlobalBounds().contains(position) || mouseSelecting;
}

void Board::calculateDropPositions() {
	std::vector<Chess::DropPiece>& currentDropPieces = whiteToPlay ? whiteDropPieces : blackDropPieces;
	const Chess::Region& currentDropRegion = whiteToPlay ? variant->whiteDropRegion : variant->blackDropRegion;
	const Chess::Region& opposingPromotionRegion = whiteToPlay ? variant->whitePromotionRegion : variant->blackPromotionRegion;
	size_t count = 0, count_2 = 0;
	for (auto& sq : currentDropRegion.getSquares(variant->boardSize)) {
		if ((sq.x + sq.y) % 2 == 0 && !Chess::getPieceFromPosition(sq, position.pieceList)) {
			count++;
		}
	}
	for (auto& sq : currentDropRegion.getSquares(variant->boardSize)) {
		if ((sq.x + sq.y) % 2 == 1 && !Chess::getPieceFromPosition(sq, position.pieceList)) {
			count_2++;
		}
	}
	for (auto& piece : currentDropPieces) {
		auto& dropSquares = piece.dropSquares;
		dropSquares.clear();
		if (piece.count != 0) {	
			for (auto& pos : currentDropRegion.getSquares(variant->boardSize)) {
				if (currentDropRegion.contains(pos) && !getPieceFromCurrentPosition(pos) &&
					(!((variant->pawnPieces.has(piece.id) || variant->promotionPawnPieces.has(piece.id)) && !variant->firstRankPawnDrops)
						|| (pos.y != 1 && pos.y != variant->boardSize.y))) {
					if (variant->promotionZonePawnDrops || (!(variant->pawnPieces.has(piece.id) || variant->promotionPawnPieces.has(piece.id)) || !opposingPromotionRegion.contains(pos))) {
						if (variant->dropOppositeColoredBishop) {
							if (piece.id == 'b') {
								bool c = false;
								for (auto& piece : position.pieceList) {
									if (piece->id == 'b' && whiteToPlay == piece->isWhite() && currentDropRegion.contains(piece->getLocalPos())) {
										auto mod = (piece->getLocalPos().x + piece->getLocalPos().y) % 2;
										auto mod_2 = (pos.x + pos.y) % 2;
										if (mod == mod_2) {
											c = true;
											break;
										}
									}
								}
								if (c) { continue; }
							}
							else {
								auto placedBishops = std::count_if(position.pieceList.begin(), position.pieceList.end(), [&](std::shared_ptr<Piece>& p) {
									return p->id == 'b' && whiteToPlay == p->isWhite() && currentDropRegion.contains(p->getLocalPos());
									});
								auto find = std::find_if(currentDropPieces.begin(), currentDropPieces.end(), [](Chess::DropPiece& dropPiece) {return dropPiece.id == 'b'; });
								if (find != currentDropPieces.end() && placedBishops < 2) {
									auto bishops = find->count;
									if (placedBishops == 1 && bishops >= 1) {
										auto fbishop = std::find_if(position.pieceList.begin(), position.pieceList.end(), [&](std::shared_ptr<Piece>& p) {
											return p->id == 'b' && whiteToPlay == p->isWhite() && currentDropRegion.contains(p->getLocalPos()); });
										if (fbishop != position.pieceList.end()) {
											auto& bishop = *fbishop;
											auto oppositeMod = (bishop->getLocalPos().x + bishop->getLocalPos().y + 1) % 2;
											auto& count_3 = oppositeMod == 0 ? count : count_2;
											if (count_3 == 1 && (pos.x + pos.y) % 2 == oppositeMod) { continue; }
										}

									}
									else if (placedBishops == 0 && bishops >= 2) {
										if (((pos.x + pos.y) % 2 == 0 && count == 1) || ((pos.x + pos.y) % 2 == 1 && count_2 == 1)) { continue; }
									}
								}
							}
						}
						if (variant->dropNoDoubledPieces.has(piece.id)) {
							auto count = std::count_if(position.pieceList.begin(), position.pieceList.end(), [&](std::shared_ptr<Piece>& p) {
								return p->id == piece.id && whiteToPlay == p->isWhite() && p->getLocalPos().x == pos.x && currentDropRegion.contains(p->getLocalPos()); });
							if (count >= variant->dropNoDoubledCount) { continue; }
						}
						std::string moveString;
						moveString += (char)std::toupper(piece.id);
						moveString += '@' + Chess::convertPositiontoNotation(pos);
						if (variant->immobilityIllegal) {
							Chess::PColor col = whiteToPlay ? Chess::PColor::White : Chess::PColor::Black;
							pieceVector newV;
							for (size_t j = 0; j < position.pieceList.size(); j++)
							{
								newV.push_back(position.pieceList[j]->clone(false, false));
							}
							newV.push_back(std::make_shared<Piece>(piece.id, pos.x, pos.y, pieceScale, boardOffset, boardSize, variant->boardSize, boardMultiplier, col, pieceTextures->front().begin()->second, false, false, false, true));
							Chess::Position newPos = position;
							newPos.pieceList = newV;
							newPos.extinctionRoyalPieces = Chess::getExtinctionRoyalPieces(*variant, newV);
							auto p = Chess::getPieceFromPosition(pos, newPos.pieceList);
							if (p && variant->pieceMoves.find(p->id) != variant->pieceMoves.end()) {
								const Chess::MoveSetVector& moveSets = variant->pieceMoves.find(p->id)->second;
								Chess::SquareSet moves;
								for (const auto& set : moveSets) {
									auto pieceMoves = set->getSquares({}, p->color, p->getLocalPos(), variant->boardSize, variant->janggiCannons.has(p->id));
									moves.merge(pieceMoves.moveSet);
								}
								if (moves.empty()) { continue; }
							}
						}
						if (variant->checksEnabled) {
							Chess::PColor col = whiteToPlay ? Chess::PColor::White : Chess::PColor::Black;
							pieceVector newV;
							for (size_t j = 0; j < position.pieceList.size(); j++)
							{
								newV.push_back(position.pieceList[j]->clone(false, false));
							}
							newV.push_back(std::make_shared<Piece>(piece.id, pos.x, pos.y, pieceScale, boardOffset, boardSize, variant->boardSize, boardMultiplier, col, pieceTextures->front().begin()->second, false, false, false, true));
							Chess::Position newPos = position;
							newPos.pieceList = newV;
							newPos.extinctionRoyalPieces = Chess::getExtinctionRoyalPieces(*variant, newV);
							if (isValidPosition(newPos, *variant, col)) {
								dropSquares.emplace_back(pos, moveString);
							}
						}
						else {
							dropSquares.emplace_back(pos, moveString);
						}
					}
				}
			}
		}
	}
}

Chess::Endgame Board::determineEnd() {
	// Neither 0, Stalemate 1, Checkmate 2, Insufficient Material 3, 50 Move Rule 4, Threefold Repetition 5
	if (!variant->whiteFlagRegion.empty() || !variant->blackFlagRegion.empty()) {
		bool whiteFlag = false, blackFlag = false;
		if (std::any_of(position.pieceList.begin(), position.pieceList.end(), [&](std::shared_ptr<Piece>& piece)
			{ return piece->isWhite() && variant->whiteFlagPieces.has(piece->id) && variant->whiteFlagRegion.contains(piece->getLocalPos()); })) {
			whiteFlag = true;
		}
		if (std::any_of(position.pieceList.begin(), position.pieceList.end(), [&](std::shared_ptr<Piece>& piece)
			{ return piece->isBlack() && variant->blackFlagPieces.has(piece->id) && variant->blackFlagRegion.contains(piece->getLocalPos()); })) {
			blackFlag = true;
		}
		if (whiteFlag && blackFlag) {
			return Chess::Endgame::Draw;
		}
		else if (blackFlag) {
			return whiteToPlay ? Chess::Endgame::VariantLoss : Chess::Endgame::VariantVictory;
		}
		else if (whiteFlag) {
			if (!variant->extraFlagMove) {
				return Chess::Endgame::VariantLoss;
			}
			else {
				if (!position.extraFlagMove) {
					bool canFlag = false;
					for (auto& piece : position.pieceList) {
						if (piece->isBlack() && variant->blackFlagPieces.has(piece->id)) {
							for (auto& sq : piece->getMoveSquares()) {
								if (variant->blackFlagRegion.contains(sq.pos)) {
									canFlag = true;
									break;
								}
							}
							for (auto& sq : piece->getCaptureSquares()) {
								if (variant->blackFlagRegion.contains(sq.pos)) {
									canFlag = true;
									break;
								}
							}
							for (auto& sq : piece->getEnPassantSquares()) {
								if (variant->blackFlagRegion.contains(sq.pos)) {
									canFlag = true;
									break;
								}
							}
							for (auto& sq : piece->getCaptureCastleSquares()) {
								if (variant->blackFlagRegion.contains(sq.pos)) {
									canFlag = true;
									break;
								}
							}
						}
					}
					if (canFlag) {
						position.extraFlagMove = true;
					}
					else {
						return Chess::Endgame::VariantLoss;
					}

				}
				else { return Chess::Endgame::VariantLoss;  }
			}
		}
	}
	if (variant->extinctionValue != Chess::WinValue::None) {
		auto& currentExtinctionPieces = whiteToPlay ? variant->whiteExtinctionPieces : variant->blackExtinctionPieces;
		if (currentExtinctionPieces.has('*')) {
			if (!variant->extinctionPseudoRoyal) {
				size_t count = std::count_if(position.pieceList.cbegin(), position.pieceList.cend(), [whiteToPlay = whiteToPlay](const std::shared_ptr<Piece>& piece)
					{ return whiteToPlay == piece->isWhite(); });
				if (count <= variant->extinctionPieceCount) {
					switch (variant->extinctionValue) {
					case Chess::WinValue::Draw:
						return Chess::Endgame::Draw;
						break;
					case Chess::WinValue::Loss:
						return Chess::Endgame::VariantLoss;
						break;
					case Chess::WinValue::Win:
						return Chess::Endgame::VariantVictory;
						break;
					}
				}
			}
		}
		else {
			for (auto& p : currentExtinctionPieces) {
				size_t count = std::count(variant->startingFEN.begin(), variant->startingFEN.end(), p);
				if (variant->extinctionPseudoRoyal && count <= variant->extinctionPieceCount) { continue; }
				if (std::count_if(position.pieceList.cbegin(), position.pieceList.cend(), [&, p](const std::shared_ptr<Piece>& piece)
					{ return whiteToPlay == piece->isWhite() && piece->id == p; }) <= variant->extinctionPieceCount) {
					switch (variant->extinctionValue) {
					case Chess::WinValue::Draw:
						return Chess::Endgame::Draw;
						break;
					case Chess::WinValue::Loss:
						return Chess::Endgame::VariantLoss;
						break;
					case Chess::WinValue::Win:
						return Chess::Endgame::VariantVictory;
						break;
					}
				}
			}
		}
	}
	if (variant->nMoveRule != 0 && halfMoves >= variant->nMoveRule) {
		return Chess::Endgame::FiftyMoveRule;
	}
	if (variant->nFoldRule != 0) {
		for (auto& p : allPositionsPlayed) {
			if (p.second >= variant->nFoldRule) {
				switch (variant->nFoldValue) {
				case Chess::WinValue::Draw:
					return Chess::Endgame::Draw;
					break;
				case Chess::WinValue::Loss:
					return Chess::Endgame::VariantVictory;
					break;
				case Chess::WinValue::Win:
					return Chess::Endgame::VariantLoss;
					break;
				}
			}
		}
	}
	if (variant->checksEnabled && variant->nCheckCounting) {
		if (whiteChecksLeft <= 0 || blackChecksLeft <= 0) {
			return Chess::Endgame::VariantLoss;
		}	
	}
	if (variant->dropsEnabled) {
		std::vector<Chess::DropPiece>& currentDP = whiteToPlay ? whiteDropPieces : blackDropPieces;
		std::vector<Chess::DropPiece> currentDropPieces;
		for (auto& d : currentDP) { if (d.count > 0) { currentDropPieces.push_back(d); } }
		if (std::none_of(position.pieceList.begin(), position.pieceList.end(), [whiteToPlay = this->whiteToPlay](std::shared_ptr<Piece>& piece) { return (whiteToPlay == piece->isWhite()) && piece->canMove; }) &&
			(currentDropPieces.empty() || std::all_of(currentDropPieces.begin(), currentDropPieces.end(), [](Chess::DropPiece& dp) { return dp.dropSquares.empty(); }))) {
			for (auto& piece : position.pieceList) {
				if (whiteToPlay == (piece->color == Chess::PColor::White)) {
					if (piece->inCheck) {
						switch (variant->checkmateValue) {
						case Chess::WinValue::Draw:
							return Chess::Endgame::Draw;
							break;
						case Chess::WinValue::Loss:
							return Chess::Endgame::Checkmate;
							break;
						case Chess::WinValue::Win:
							return Chess::Endgame::VariantVictory;
							break;
						}
					}
				}
			}
			switch (variant->stalemateValue) {
			case Chess::WinValue::Draw:
				return Chess::Endgame::Draw;
				break;
			case Chess::WinValue::Loss:
				return Chess::Endgame::VariantLoss;
				break;
			case Chess::WinValue::Win:
				return Chess::Endgame::VariantVictory;
				break;
			}
		}
	}
	else {
		if (std::none_of(position.pieceList.begin(), position.pieceList.end(), [whiteToPlay = this->whiteToPlay](std::shared_ptr<Piece>& piece) { return (whiteToPlay == piece->isWhite()) && piece->canMove; })) {
			for (auto& piece : position.pieceList) {
				if (whiteToPlay == (piece->isWhite())) {
					if (piece->inCheck) {
						switch (variant->checkmateValue) {
						case Chess::WinValue::Draw:
							return Chess::Endgame::Draw;
							break;
						case Chess::WinValue::Loss:
							return Chess::Endgame::VariantLoss;
							break;
						case Chess::WinValue::Win:
							return Chess::Endgame::VariantVictory;
							break;
						}
					}
				}
			}
		}
		if (variant->stalemateValue != Chess::WinValue::None) {
			if (std::none_of(position.pieceList.begin(), position.pieceList.end(), [whiteToPlay = this->whiteToPlay](std::shared_ptr<Piece>& piece) { return (whiteToPlay == piece->isWhite()) && piece->canMove; })) {
				for (auto& piece : position.pieceList) {
					if (whiteToPlay == (piece->color == Chess::PColor::White)) {
						switch (variant->stalemateValue) {
						case Chess::WinValue::Draw:
							return Chess::Endgame::Draw;
							break;
						case Chess::WinValue::Loss:
							return Chess::Endgame::VariantLoss;
							break;
						case Chess::WinValue::Win:
							return Chess::Endgame::VariantVictory;
							break;
						}
					}
				}
			}
		}
	}
	return Chess::Endgame::None;
}

void Board::setVariant(size_t variantIndex)
{
	if (variant && variant->name == "chess960") { position.hasChess960 = false; }
	this->variant = variantList->at(variantIndex).get();
	this->variantIndex = variantIndex;
	if (variant->name == "chess960") { position.hasChess960 = true; }
	boardTexture = Chess::loadBoard(*boardSheet, boardSet, variant->boardSize);
	boardSprite.setTexture(boardTexture, true);
	if (variantPieceTextures->find(variant->name) != variantPieceTextures->end()) {
		currentPieceTextures = &variantPieceTextures->find(variant->name)->second;
	}
	else { currentPieceTextures = &pieceTextures->at(pieceSet); }
	setPieceTextures();
	// Vars
	float ScaleX = windowSize.x / (float)boardTexture.getSize().x; // + Crazyhouse
	float ScaleY = windowSize.y / (float)boardTexture.getSize().y;
	boardScale = std::roundf(std::min(ScaleX, ScaleY) * sizeMultiplier * 1000.0f) / 1000.0f; // Round to 0.00
	startingScale = boardScale;
	pieceScale = boardScale * (128.0f / static_cast<float>(currentPieceTextures->begin()->second.getSize().x));
	boardMultiplier = boardScale * 128.0f;
	boardOffset.x = (windowSize.x / 2.0f) - ((boardTexture.getSize().x * boardScale) / 2.0f) + startingOffset.x;
	boardOffset.y = (windowSize.y / 2.0f) - ((boardTexture.getSize().y * boardScale) / 2.0f) + startingOffset.y;
	this->boardSize = { (boardTexture.getSize().x * boardScale), (boardTexture.getSize().y * boardScale) };
	offset += startingOffset;
	
	for (auto& sprite : checkSprites) {
		sprite.setScale({ boardScale, boardScale });
	}
	selectedPieceBackground.setScale({ boardScale, boardScale });
	lastMoveStart.setScale({ boardScale, boardScale });
	lastMoveDest.setScale({ boardScale, boardScale });
	kothBackground.setScale({ boardScale, boardScale });
	kothShadow.setScale({ boardScale, boardScale });
	rankBackground.setScale({ boardScale, boardScale });
	rankShadowTop.setScale({ boardScale, boardScale });
	whiteCheckCount.setScale({ boardScale / 3.0f, boardScale / 3.0f });
	blackCheckCount.setScale({ boardScale / 3.0f, boardScale / 3.0f });
	whiteCheckText.setScale({ boardScale, boardScale });
	blackCheckText.setScale({ boardScale, boardScale });
	promotionOverlay.setPosition(boardSprite.getPosition());
	optionChangeOverlay.setPosition(boardSprite.getPosition());
	gameEndOverlay.setPosition(boardSprite.getPosition());
	dropPieceBackgroundW.setPosition({ boardOffset.x - (dropPieceSquareSize.x / 1.25f), dropPieceBackgroundW.getPosition().y });
	dropPieceBackgroundB.setPosition({ boardOffset.x + (dropPieceSquareSize.y / 1.25f + boardTexture.getSize().x * boardScale), dropPieceBackgroundW.getPosition().y });
	ghostSprite.setScale({ pieceScale, pieceScale });

	blackTimeBG.setScale({ boardScale * 1.02f, boardScale * 1.02f });
	blackTimeBG.setPosition({ getGlobalPosition(sf::Vector2f{variant->boardSize.x * 6.5f / 8.0f, variant->boardSize.y + 0.5f}, false) });
	blackTimeBG.move({ 0, -(blackTimeBG.getTexture().getSize().y * blackTimeBG.getScale().y / 2.0f) - 1 });
	blackTimeOutline.setScale({ boardScale * 1.11f, boardScale * 1.129f });
	blackTimeOutline.setPosition({ getGlobalPosition(sf::Vector2f{variant->boardSize.x * 6.5f / 8.0f, variant->boardSize.y + 0.5f}, false) });
	blackTimeOutline.move({ 0, -(blackTimeOutline.getTexture().getSize().y * blackTimeOutline.getScale().y / 2.0f) - 1 });
	blackTimeText.setPosition({ blackTimeBG.getPosition() });
	blackTimeText.setScale({ boardScale, boardScale });
	whiteTimeBG.setScale({ boardScale * 1.02f, boardScale * 1.02f });
	whiteTimeBG.setPosition({ getGlobalPosition(sf::Vector2f{variant->boardSize.x * 2.5f / 8.0f, variant->boardSize.y + 0.5f}, false) });
	whiteTimeBG.move({ 0, -(whiteTimeBG.getTexture().getSize().y * whiteTimeBG.getScale().y / 2.0f) - 1 });
	whiteTimeOutline.setScale({ boardScale * 1.11f, boardScale * 1.129f });
	whiteTimeOutline.setPosition({ getGlobalPosition(sf::Vector2f{variant->boardSize.x * 2.5f / 8.0f, variant->boardSize.y + 0.5f}, false) });
	whiteTimeOutline.move({ 0, -(whiteTimeOutline.getTexture().getSize().y * whiteTimeOutline.getScale().y / 2.0f) - 1 });
	whiteTimeText.setPosition({ whiteTimeBG.getPosition() });
	whiteTimeText.setScale({ boardScale, boardScale });

	promotePieceSize = { pieceScale * 0.82f, pieceScale * 0.82f };
	boardSprite.setScale({ boardScale, boardScale });
	boardSprite.setOrigin(boardSprite.getLocalBounds().getCenter());
	promotionOverlay.setSize((sf::Vector2f)boardTexture.getSize());
	promotionOverlay.setOrigin(promotionOverlay.getLocalBounds().getCenter());
	promotionOverlay.setScale(boardSprite.getScale());
	optionChangeOverlay.setSize((sf::Vector2f)boardTexture.getSize());
	optionChangeOverlay.setOrigin(optionChangeOverlay.getLocalBounds().getCenter());
	optionChangeOverlay.setScale(boardSprite.getScale());
	optionChangeOverlay.setFillColor(sf::Color(0, 0, 0, 0));
	gameEndText.setPosition(boardSprite.getPosition());
	gameEndText.setScale(boardSprite.getScale() * (boardSprite.getTexture().getSize().x / 1024.0f));
	gameEndText.setOrigin({ gameEndText.getLocalBounds().position.x + (gameEndText.getLocalBounds().size.x / 2.0f), gameEndText.getLocalBounds().position.y + (gameEndText.getLocalBounds().size.y / 2.0f) });
	gameEndOverlay.setSize((sf::Vector2f)boardTexture.getSize());
	gameEndOverlay.setOrigin(gameEndOverlay.getLocalBounds().getCenter());
	gameEndOverlay.setScale(boardSprite.getScale());
	gameEndOverlay.setFillColor(sf::Color(0, 0, 0, 0));
	gameEndText.setPosition(boardSprite.getPosition());
	gameEndText.setScale(boardSprite.getScale() * (boardSprite.getTexture().getSize().x / 1024.0f));
	gameEndText.setOrigin({ gameEndText.getLocalBounds().position.x + (gameEndText.getLocalBounds().size.x / 2.0f), gameEndText.getLocalBounds().position.y + (gameEndText.getLocalBounds().size.y / 2.0f) });
	setSpritePositions();
	scale(1);

	if (!stockfishEnabled) { AI_Only = false; }
	loadFromFEN({}, true);
}

// Move Functions
void Board::playMove(std::string& moveString, bool instantMove, bool promoteMove) {
	if (moveString.empty() || moveString == "(none)") {
		std::cout << "Got Empty Move String!" << std::endl;
		playingMove = false;
		return;
	}
	else if (moveString == "(none)\r") {
		std::cout << "Got Empty Move String!" << std::endl;
		std::cout << "Game Should End" << std::endl;
		playingMove = false;
		return;
	}
	else {
		selectedPiece.reset();
		capturePiece.reset();
		auto enPassantTargetT = position.enPassantTarget;
		auto enPassantTripleTargetT = position.enPassantTripleTarget;
		position.enPassantTarget = {};
		position.enPassantTripleTarget = {};

		if (moveString.back() == '\r') {
			moveString.pop_back();
		}

		// Drop Pieces
		if (moveString.at(1) == '@') {
			moves += " " + moveString;
			sf::Vector2i dest = Chess::convertChessNotationtoPosition(moveString.substr(2));
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

			std::shared_ptr<Piece> newPiece = getPieceFromID(moveString.front(), dest, col, false, false);
			if (variant->gating && variant->seirawanGating) { newPiece->hasMoved = true; }
			position.pieceList.push_back(newPiece);
			if (variant->castlingDroppedPiece) {
				int offset = newPiece->isWhite() ? 0 : 2;
				if (variant->castlingKingPiece.has(newPiece->id) && ((newPiece->getLocalPos().x == variant->castlingKingFile) || position.hasChess960)) {
					position.castlingRights[offset] = true;
					position.castlingRights[offset + 1] = true;
				}
				if (variant->castlingRookPieces.has(newPiece->id)) {
					auto piece = getPieceFromCurrentPosition({ variant->castlingKingFile, newPiece->getLocalPos().y });
					if (piece && piece->color == newPiece->color && variant->castlingKingPiece.has(piece->id) && piece->getLocalPos() != newPiece->getLocalPos()) {
						if (newPiece->getLocalPos().x < piece->getLocalPos().y) {
							position.castlingRights[offset + 1] = true;
						}
						else {
							position.castlingRights[offset] = true;
						}
					}
				}
			}
			getCastlingRights(getCurrentFEN());
			calculatingPos = true;
			postMove(pieceVector{ newPiece }, true);
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
				std::cout << start.x << " " << start.y << std::endl;
				std::cout << moves << std::endl;
				std::cout << moveString << std::endl;
				playingMove = false;
				return;
			}
			std::shared_ptr<Piece> capture = nullptr;

			// Promotion
			if (!std::isdigit(moveString.back())) {
				std::shared_ptr<Piece> rookPiece, kingPiece;
				auto& currentDropPieces = whiteToPlay ? whiteDropPieces : blackDropPieces;
				bool closeCastle = false;
				moves += " " + moveString;
				std::string t{ moveString.begin() + midPos, moveString.end() - 1 };
				dest = Chess::convertChessNotationtoPosition(t);
				capture = getPieceFromCurrentPosition(dest);
				lastMoveStart.setPosition(getGlobalPosition(start));
				lastMoveDest.setPosition(getGlobalPosition(dest));
				if (capture && capture->color == piece->color) {
					if (variant->castlingKingPiece.has(piece->id) && variant->castlingRookPieces.has(capture->id)) {
						rookPiece = capture;
						kingPiece = piece;
						closeCastle = true;
					}
					else if (variant->castlingKingPiece.has(capture->id) && variant->castlingRookPieces.has(piece->id)) {
						rookPiece = piece;
						kingPiece = capture;
						closeCastle = true;
					}
					capture.reset();
				}

				if (capture) {
					captureSound.play();
					ghostSprite.setTexture(capture->getTexture(), true);
					ghostSprite.setPosition(capture->getGlobalPos());
					setSpriteVisible(ghostSprite, true, 75);
					capture->setVisible(false);
					capturePiece = capture;
				}
				else {
					moveSound.play();
				}
				int offset = piece->isWhite() ? 0 : 2;
				if (variant->gating && variant->seirawanGating && std::isalpha(moveString.back())) {
					if (!closeCastle) {
						int castleRank = (piece->isWhite() ? variant->castleRank : static_cast<int>(variant->boardSize.y) - variant->castleRank + 1);
						auto& moves = piece->getMoveSquares();
						auto& captureMoves = piece->getCaptureSquares();
						bool find = std::find_if(moves.begin(), moves.end(), [&](Chess::Square& sq) { return sq.pos == dest; }) != moves.end() ||
							std::find_if(captureMoves.begin(), captureMoves.end(), [&](Chess::Square& sq) { return sq.pos == dest; }) != captureMoves.end();
						if (!find) {
							if (position.castlingRights[0 + offset] && dest == sf::Vector2i{ variant->castleKDestination, castleRank }) {
								std::shared_ptr<Piece> rook = getPieceFromCurrentPosition({ position.castlePieces[0 + offset], castleRank });
								if (rook) {
									rook->setTarget(getGlobalPosition(sf::Vector2i{ variant->castleKDestination - 1, piece->getLocalPos().y }));
									piece->setTarget(getGlobalPosition(sf::Vector2i{ variant->castleKDestination, piece->getLocalPos().y }));
									rook->setPosition(rook->getTarget());
									piece->setPosition(piece->getTarget());						
									for (auto& piece : currentDropPieces) {
										if (piece.id == moveString.back() && piece.count > 0) {
											piece.count--;
										}
									}
									std::shared_ptr<Piece> newPiece = getPieceFromID(moveString.back(), start, piece->color, false, false);
									position.pieceList.push_back(newPiece);
									calculatingPos = true;
									playingMove = false;
									return postMove(pieceVector{ piece, rook, newPiece }, false);
								}
							}
							else if (position.castlingRights[1 + offset] && dest == sf::Vector2i{ variant->castleQDestination, castleRank }) {
								std::shared_ptr<Piece> rook = getPieceFromCurrentPosition({ position.castlePieces[1 + offset], castleRank });
								if (rook) {
									rook->setTarget(getGlobalPosition(sf::Vector2i{ variant->castleQDestination + 1, piece->getLocalPos().y }));
									piece->setTarget(getGlobalPosition(sf::Vector2i{ variant->castleQDestination, piece->getLocalPos().y }));
									rook->setPosition(rook->getTarget());
									piece->setPosition(piece->getTarget());
									for (auto& piece : currentDropPieces) {
										if (piece.id == moveString.back() && piece.count > 0) {
											piece.count--;
										}
									}
									std::shared_ptr<Piece> newPiece = getPieceFromID(moveString.back(), start, piece->color, false, false);
									position.pieceList.push_back(newPiece);
									calculatingPos = true;
									playingMove = false;
									return postMove(pieceVector{ piece, rook, newPiece }, false);
								}
							}
						}
					}					
					else if (kingPiece && rookPiece) {
						int pieceX = kingPiece->getLocalPos().x;
						if (position.castlingRights[0 + offset] && rookPiece->getLocalPos().x > pieceX) {
							rookPiece->setTarget(getGlobalPosition(sf::Vector2i{ variant->castleKDestination - 1, kingPiece->getLocalPos().y }));
							kingPiece->setTarget(getGlobalPosition(sf::Vector2i{ variant->castleKDestination, kingPiece->getLocalPos().y }));
							rookPiece->setPosition(rookPiece->getTarget());
							kingPiece->setPosition(kingPiece->getTarget());
							for (auto& piece : currentDropPieces) {
								if (piece.id == moveString.back() && piece.count > 0) {
									piece.count--;
								}
							}
							std::shared_ptr<Piece> newPiece = getPieceFromID(moveString.back(), start, kingPiece->color, false, false);
							position.pieceList.push_back(newPiece);
							calculatingPos = true;
							playingMove = false;
							return postMove(pieceVector{ kingPiece, rookPiece, newPiece }, false);
						}
						else if (position.castlingRights[1 + offset] && rookPiece->getLocalPos().x < pieceX) {
							rookPiece->setTarget(getGlobalPosition(sf::Vector2i{ variant->castleQDestination + 1, kingPiece->getLocalPos().y }));
							kingPiece->setTarget(getGlobalPosition(sf::Vector2i{ variant->castleQDestination, kingPiece->getLocalPos().y }));
							rookPiece->setPosition(rookPiece->getTarget());
							kingPiece->setPosition(kingPiece->getTarget());
							for (auto& piece : currentDropPieces) {
								if (piece.id == moveString.back() && piece.count > 0) {
									piece.count--;
								}
							}
							std::shared_ptr<Piece> newPiece = getPieceFromID(moveString.back(), start, kingPiece->color, false, false);
							position.pieceList.push_back(newPiece);
							calculatingPos = true;
							playingMove = false;
							return postMove(pieceVector{ kingPiece, rookPiece, newPiece }, false);
						}
					}
				}

				if (moveString.back() == '+') {
					auto find = variant->promotedPieceTypes.find(piece->id);
					if (find != variant->promotedPieceTypes.end()) {
						for (size_t i = 0; i < position.pieceList.size(); i++) {
							if (position.pieceList.at(i)->getLocalPos() == start) {
								position.pieceList.erase(position.pieceList.begin() + i);
								break;
							}
						}
						std::shared_ptr<Piece> newPiece = getPieceFromID(find->second, dest, piece->color, false, true);
						newPiece->demotedID = piece->id;
						position.pieceList.push_back(newPiece);
						calculatingPos = true;
						postMove(pieceVector{ newPiece }, false);
					}
				}
				else if (moveString.back() == '-' && piece->demotedID != ' ') {
					for (size_t i = 0; i < position.pieceList.size(); i++) {
						if (position.pieceList.at(i)->getLocalPos() == start) {
							position.pieceList.erase(position.pieceList.begin() + i);
							break;
						}
					}
					std::shared_ptr<Piece> newPiece = getPieceFromID(piece->demotedID, dest, piece->color, false, false);
					position.pieceList.push_back(newPiece);
					calculatingPos = true;
					postMove(pieceVector{ newPiece }, false);
				}
				else {
					auto& currentPromotionRegion = whiteToPlay ? variant->whitePromotionRegion : variant->blackPromotionRegion;
					if (currentPromotionRegion.contains(dest) && (variant->promotionPawnPieces.has(piece->id) || variant->pawnPieces.has(piece->id))) {
						for (size_t i = 0; i < position.pieceList.size(); i++) {
							if (position.pieceList.at(i)->getLocalPos() == start) {
								position.pieceList.erase(position.pieceList.begin() + i);
								break;
							}
						}
						std::shared_ptr<Piece> newPiece = getPieceFromID(moveString.back(), dest, piece->color, true, false);
						newPiece->demotedID = piece->id;
						position.pieceList.push_back(newPiece);
						calculatingPos = true;
						postMove(pieceVector{newPiece}, false);
					}
					else if (variant->gating && variant->seirawanGating) {
						for (auto& piece : currentDropPieces) {
							if (piece.id == moveString.back() && piece.count > 0) {
								piece.count--;
							}
						}
						std::shared_ptr<Piece> newPiece = getPieceFromID(moveString.back(), start, piece->color, false, false);
						position.pieceList.push_back(newPiece);
						piece->setTarget(getGlobalPosition(dest));
						piece->setPosition(getGlobalPosition(dest));
						postMove(pieceVector{ piece, newPiece }, false);
					}
				}
			}

			else {
				dest = Chess::convertChessNotationtoPosition(moveString.substr(midPos));
				capture = getPieceFromCurrentPosition(dest);
				bool closeCastle = false;
				if (capture) {
					if (capture->color == piece->color) {
						if (variant->castlingKingPiece.has(piece->id) && variant->castlingRookPieces.has(capture->id)) {
							closeCastle = true;
						}	
						capture.reset();
					}
				}
				piece->setTarget(getGlobalPosition(dest));
				if (instantMove) {
					piece->setPosition(piece->getTarget());
				}
				
				if (variant->pawnPieces.has(piece->id) || variant->enPassantPieces.has(piece->id)) {
					if (piece->color == Chess::PColor::White) {
						if (dest.x == piece->getLocalPos().x && dest.y == piece->getLocalPos().y + 2) {
							position.enPassantTarget = dest;
						}
						if (dest.x == piece->getLocalPos().x && dest.y == piece->getLocalPos().y + 3) {
							position.enPassantTarget = dest;
							position.enPassantTripleTarget = dest;
						}
						if (!capture && enPassantTargetT.has_value()
							&& (enPassantTargetT.value() == sf::Vector2i{ dest.x, dest.y - 1 })) {
							auto enPassant = getPieceFromCurrentPosition(enPassantTargetT.value());
							if (enPassant && enPassant->color != piece->color) {
								capture = enPassant;
							}
						}
						if (!capture && enPassantTripleTargetT.has_value()
							&& (enPassantTripleTargetT.value() == sf::Vector2i{ dest.x, dest.y - 2 })) {
							auto enPassant = getPieceFromCurrentPosition(enPassantTripleTargetT.value());
							if (enPassant && enPassant->color != piece->color) {
								capture = enPassant;
							}
						}
					}
					else {
						if (dest.x == piece->getLocalPos().x && dest.y == piece->getLocalPos().y - 2) {
							position.enPassantTarget = dest;
						}
						if (dest.x == piece->getLocalPos().x && dest.y == piece->getLocalPos().y - 3) {
							position.enPassantTarget = dest;
							position.enPassantTripleTarget = dest;
						}
						if (!capture && enPassantTargetT.has_value()
							&& (enPassantTargetT.value() == sf::Vector2i{ dest.x, dest.y + 1 })) {
							auto enPassant = getPieceFromCurrentPosition(enPassantTargetT.value());
							if (enPassant && enPassant->color != piece->color) {
								capture = enPassant;
							}
						}
						if (!capture && enPassantTripleTargetT.has_value()
							&& (enPassantTripleTargetT.value() == sf::Vector2i{ dest.x, dest.y + 2 })) {
							auto enPassant = getPieceFromCurrentPosition(enPassantTripleTargetT.value());
							if (enPassant && enPassant->color != piece->color) {
								capture = enPassant;
							}
						}
					}
				}
				bool isPiecePromotion = (variant->promotedPieceTypes.find(piece->id) != variant->promotedPieceTypes.end()) || (variant->pieceDemotion && piece->promotedPieceType && piece->demotedID != ' ');
				if (isPiecePromotion) {
					const auto& currentPromotionRegion = piece->isWhite() ? variant->whitePromotionRegion : variant->blackPromotionRegion;
					Chess::PieceType* validTypes = nullptr;
					for (auto& sq : piece->getMoveSquares()) {
						if (sq.promoteSquare && sq.pos == dest) {
							validTypes = &sq.validPromotionTypes;
							break;
						}
					}
					if (!validTypes) {
						for (auto& sq : piece->getCaptureSquares()) {
							if (sq.promoteSquare && sq.pos == dest) {
								validTypes = &sq.validPromotionTypes;
								break;
							}
						}
					}
					if (validTypes && (currentPromotionRegion.contains(dest) || (currentPromotionRegion.contains(start))) && !validTypes->empty()) {
						if (promoteMove) {
							promotePiece = piece;
							shouldPostMove = false;
							if (capture) {
								ghostSprite.setTexture(capture->getTexture(), true);
								ghostSprite.setPosition(capture->getGlobalPos());
								setSpriteVisible(ghostSprite, true, 75);
								capture->setVisible(false);
								capturePiece = capture;
							}
							checkSprites.clear();
							return;
						}
						else if (!validTypes->has(piece->id) && validTypes->count() == 1) {
							if (variant->promotedPieceTypes.find(piece->id) != variant->promotedPieceTypes.end() && variant->promotedPieceTypes.find(piece->id)->second == *validTypes->begin()) {
								std::string m = (moveString + '+');
								playMove(m, instantMove);
								return;
							}
							else if (piece->promotedPieceType && piece->demotedID == *validTypes->begin()) {
								std::string m = (moveString + '-');
								playMove(m, instantMove);
								return;
							}

						}
					}

				}
				if (variant->pawnPieces.has(piece->id) || variant->promotionPawnPieces.has(piece->id)) {
					const auto& currentPromotionRegion = piece->isWhite() ? variant->whitePromotionRegion : variant->blackPromotionRegion;
					const auto& currentPromotedPieces = piece->isWhite() ? variant->whitePromotePieces : variant->blackPromotePieces;
					Chess::PieceType* validTypes = nullptr;
					for (auto& sq : piece->getMoveSquares()) {
						if (sq.promoteSquare && sq.pos == dest) {
							validTypes = &sq.validPromotionTypes;
							checkSprites.clear();
							break;
						}
					}
					if (!validTypes) {
						for (auto& sq : piece->getCaptureSquares()) {
							if (sq.promoteSquare && sq.pos == dest) {
								validTypes = &sq.validPromotionTypes;
								break;
							}
						}
					}
					if (validTypes && currentPromotionRegion.contains(dest) && !currentPromotedPieces.empty() && !validTypes->empty()) {
						if (promoteMove) {
							promotePiece = piece;
							shouldPostMove = false;
							if (capture) {
								ghostSprite.setTexture(capture->getTexture(), true);
								ghostSprite.setPosition(capture->getGlobalPos());
								setSpriteVisible(ghostSprite, true, 75);
								capture->setVisible(false);
								capturePiece = capture;
							}
							checkSprites.clear();
							return;
						}
						else if (!validTypes->has(piece->id) && validTypes->count() == 1) {
							std::string m = (moveString + *validTypes->begin());
							playMove(m, instantMove);
							return;
						}
					}
				}

				// Castles
				else if (variant->castlingKingPiece.has(piece->id)) {
					int pieceX = piece->getLocalPos().x, offset = piece->isWhite() ? 0 : 2;
					if (!closeCastle) {
						int castleRank = (piece->isWhite() ? variant->castleRank : static_cast<int>(variant->boardSize.y) - variant->castleRank + 1);
						auto& moves = piece->getMoveSquares();
						auto& captureMoves = piece->getCaptureSquares();
						bool find = std::find_if(moves.begin(), moves.end(), [&](Chess::Square& sq) { return sq.pos == dest; }) != moves.end() ||
							std::find_if(captureMoves.begin(), captureMoves.end(), [&](Chess::Square& sq) { return sq.pos == dest; }) != captureMoves.end();
						if (!find) {
							if (position.castlingRights[offset] && dest == sf::Vector2i{ variant->castleKDestination, castleRank }) {
								std::shared_ptr<Piece> rook = getPieceFromCurrentPosition({ position.castlePieces[offset], castleRank });
								if (rook) {
									rook->setTarget(getGlobalPosition(sf::Vector2i{ variant->castleKDestination - 1, piece->getLocalPos().y }));
									piece->setTarget(getGlobalPosition(sf::Vector2i{ variant->castleKDestination, piece->getLocalPos().y }));
									if (instantMove) {
										rook->setPosition(rook->getTarget());
										piece->setPosition(piece->getTarget());
									}
									if (promoteMove && variant->gating && variant->seirawanGating && !piece->hasMoved && !rook->hasMoved) {
										gatingKing = piece;
										gatingRook = rook;
									}
									else {
										castleKing = piece;
										castleRook = rook;
									}
								}
							}
							else if (position.castlingRights[1 + offset] && dest == sf::Vector2i{ variant->castleQDestination, castleRank }) {
								std::shared_ptr<Piece> rook = getPieceFromCurrentPosition({ position.castlePieces[1 + offset], castleRank });
								if (rook) {
									rook->setTarget(getGlobalPosition(sf::Vector2i{ variant->castleQDestination + 1, piece->getLocalPos().y }));
									piece->setTarget(getGlobalPosition(sf::Vector2i{ variant->castleQDestination, piece->getLocalPos().y }));
									if (instantMove) {
										rook->setPosition(rook->getTarget());
										piece->setPosition(piece->getTarget());
									}
									if (promoteMove && variant->gating && variant->seirawanGating && !piece->hasMoved && !rook->hasMoved) {
										gatingKing = piece;
										gatingRook = rook;
									}
									else {
										castleKing = piece;
										castleRook = rook;
									}
								}
							}
						}
					}
					else {
						if (position.castlingRights[0 + offset] && dest.x > pieceX) {
							std::shared_ptr<Piece> rook = getPieceFromCurrentPosition({ position.castlePieces[0 + offset], piece->getLocalPos().y });
							if (rook) {
								rook->setTarget(getGlobalPosition(sf::Vector2i{ variant->castleKDestination - 1, piece->getLocalPos().y }));
								piece->setTarget(getGlobalPosition(sf::Vector2i{ variant->castleKDestination, piece->getLocalPos().y }));
								if (instantMove) {
									rook->setPosition(rook->getTarget());
									piece->setPosition(piece->getTarget());
								}
								if (promoteMove && variant->gating && variant->seirawanGating && !piece->hasMoved && !rook->hasMoved) {
									gatingKing = piece;
									gatingRook = rook;
								}
								else {
									castleKing = piece;
									castleRook = rook;
								}
							}
						}
						else if (position.castlingRights[1 + offset] && dest.x < pieceX) {
							std::shared_ptr<Piece> rook = getPieceFromCurrentPosition({ position.castlePieces[1 + offset], piece->getLocalPos().y });
							if (rook) {
								rook->setTarget(getGlobalPosition(sf::Vector2i{ variant->castleQDestination + 1, piece->getLocalPos().y }));
								piece->setTarget(getGlobalPosition(sf::Vector2i{ variant->castleQDestination, piece->getLocalPos().y }));
								if (instantMove) {
									rook->setPosition(rook->getTarget());
									piece->setPosition(piece->getTarget());
								}
								if (promoteMove && variant->gating && variant->seirawanGating && !piece->hasMoved && !rook->hasMoved) {
									gatingKing = piece;
									gatingRook = rook;
								}
								else {
									castleKing = piece;
									castleRook = rook;
								}
							}
						}
					}
				}
				if (variant->gating && variant->seirawanGating && piece->getLocalPos().y == (piece->isWhite() ? 1 : variant->boardSize.y) && !piece->hasMoved) {
					if (promoteMove && !gatingKing && !gatingRook) {
						gatingPiece = piece; 
						shouldPostMove = false;
						if (capture) {
							ghostSprite.setTexture(capture->getTexture(), true);
							ghostSprite.setPosition(capture->getGlobalPos());
							setSpriteVisible(ghostSprite, true, 75);
							capture->setVisible(false);
							capturePiece = capture;
						}
						checkSprites.clear();
						return;
					}
				}
				if (capture) {
					ghostSprite.setTexture(capture->getTexture(), true);
					ghostSprite.setPosition(capture->getGlobalPos());
					setSpriteVisible(ghostSprite, true, 75);
					capture->setVisible(false);
					capturePiece = capture;
				}

				if (!promotePiece && !gatingPiece && !gatingRook && !gatingKing) {
					moves += " " + moveString;
					lastMoveStart.setPosition(getGlobalPosition(piece->getLocalPos()));
					lastMoveDest.setPosition(getGlobalPosition(dest));
					if (capture) {
						captureSound.play();
					}
					else {
						moveSound.play();
					}
				}
				checkSprites.clear();
			}
		}
	}
	playingMove = false;
}

void Board::postMove(pieceVector movePieces, bool drop) {
	movesPlayed++;
	if (timeEnabled && timeIncrement != sf::Time::Zero) {
		if (whiteToPlay) { whiteTime += timeIncrement; }
		else { blackTime += timeIncrement; }
	}
	whiteToPlay = !whiteToPlay;
	hasCheck = false;
	lastMoveStartLocal = getLocalPosition(lastMoveStart.getPosition());
	lastMoveDestLocal = getLocalPosition(lastMoveDest.getPosition());
	halfMoves++;

	if (movePieces.empty()) {
		calculatingPos = false;
		return;
	}
	else if (movePieces.front()->isBlack()) { fullMoves++; }

	// Captures
	if (capturePiece) {
		halfMoves = 0;
		setSpriteVisible(ghostSprite, false);
		std::shared_ptr<Piece>& piece = movePieces.front();
		sf::Vector2i cpos = getLocalPosition(piece->getGlobalPos());
		for (auto it = position.pieceList.begin(); it != position.pieceList.end(); it++) {
			if (*it == capturePiece) {
				position.pieceList.erase(it);
				break;
			}
		}
		if (variant->atomicExplosions) {
			std::vector<sf::Vector2i> piecePositions;
			if (!variant->atomicImmunePieces.has(piece->id)) {
				piecePositions.push_back(piece->getLocalPos());
			}
			for (int x = -1; x <= 1; x++) {
				for (int y = -1; y <= 1; y++) {
					sf::Vector2i newPos = { cpos.x + x, cpos.y + y };
					if (Chess::isValidSquare(newPos, variant->boardSize)) {
						if ((x != 0 || y != 0) && Chess::isPieceAt(newPos, position.pieceList)) {
							std::shared_ptr<Piece> p = getPieceFromCurrentPosition(newPos);
							if (!variant->atomicImmunePieces.has(p->id) && !variant->pawnPiecesBlastImmunity.has(p->id)) {
								piecePositions.push_back(newPos);
							}
						}
						if (atomicExplosionEffect) {
							sf::RectangleShape atomicRect{ {boardMultiplier, boardMultiplier } };
							atomicRect.setOrigin(atomicRect.getLocalBounds().getCenter());
							atomicRect.setPosition(getGlobalPosition(newPos));
							atomicRect.setFillColor(sf::Color{ 255, 255, 255, 150 });
							captureObjects.push_back(std::move(atomicRect));
						}
					}
				}
			}
			for (auto& pos : piecePositions) {
				for (auto it = position.pieceList.begin(); it != position.pieceList.end(); it++) {
					std::shared_ptr<Piece> p = *it;
					if (p->getLocalPos() == pos) {
						position.pieceList.erase(it);
						break;
					}
				}
			}
			if (atomicExplosionEffect) { atomicClock.restart(); }
		}

		if (variant->dropsEnabled && variant->capturesToHand) {
			std::vector<Chess::DropPiece>& currentDropPieces = piece->isWhite() ? whiteDropPieces : blackDropPieces;
			if (!variant->dropLoop && capturePiece->promoted && capturePiece->demotedID != ' ') {
				for (auto& t : currentDropPieces) {
					if (t.id == capturePiece->demotedID) {
						t.count += 1;
						break;
					}
				}
			}
			else if (!variant->dropLoop && capturePiece->promotedPieceType && capturePiece->demotedID != ' ') {
				for (auto& t : currentDropPieces) {
					if (t.id == capturePiece->demotedID) {
						t.count += 1;
						break;
					}
				}
			}
			else {
				for (auto& t : currentDropPieces) {
					if (t.id == capturePiece->id) {
						t.count += 1;
						break;
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
		if (!drop && !mPiece->hasMoved) { mPiece->hasMoved = true; }
		if (variant->pawnPieces.has(mPiece->id) || variant->nMoveRulePieces.has(mPiece->id)) { halfMoves = 0; }
	}

	for (auto& piece : position.pieceList) {
		if (variant->castlingKingPiece.has(piece->id)) {
			int offset = piece->isWhite() ? 0 : 2;
			std::shared_ptr<Piece> kRook = getPieceFromCurrentPosition({ position.castlePieces[offset], piece->getLocalPos().y });
			std::shared_ptr<Piece> qRook = getPieceFromCurrentPosition({ position.castlePieces[1 + offset], piece->getLocalPos().y });
			if (piece->hasMoved || !kRook || !variant->castlingRookPieces.has(kRook->id) || kRook->hasMoved) { position.castlingRights[offset] = false; }
			if (piece->hasMoved || !qRook || !variant->castlingRookPieces.has(qRook->id) || qRook->hasMoved) { position.castlingRights[1 + offset] = false; }
		}
	}

	if (autoFlip && stockfishEnabled && whiteToPlay == isFlipped) {
		flipBoard();
	}
	generateLegalMoves(false);
	std::cout << movesPlayed << " Moves Played." << std::endl;
	calculatingPos = false;
}