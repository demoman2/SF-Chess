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

void Board::handleEvent(const std::optional<sf::Event>& event, sf::Vector2f mousePos, const sf::Image& boardSpriteSheet, bool isFocused, bool controlClicked)
{
	if (!isFocused) { return; }
	if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
	{
		if (mouseButtonPressed->button == sf::Mouse::Button::Left)
		{
			mouseClicked = true;
			if (timeEnabled) {
				if (whiteTimeBG.getGlobalBounds().contains(mousePos)) {
					if (controlClicked) {
						startingWhiteTime += sf::seconds(10);
						setOptionChange("Initial White Time", startingWhiteTime.asSeconds());
					}
					else {
						whiteTime += sf::seconds(10);
						if (stockfish.isCalculating && !stockfish.getSettings().usesFixedTime) { stockfish.stopCalculating(); }
					}
				}
				else if (blackTimeBG.getGlobalBounds().contains(mousePos)) {
					if (controlClicked) {
						startingBlackTime += sf::seconds(10);
						setOptionChange("Initial Black Time", startingBlackTime.asSeconds());
					}
					else {
						blackTime += sf::seconds(10);
						if (stockfish.isCalculating && !stockfish.getSettings().usesFixedTime) { stockfish.stopCalculating(); }
					}
				}
			}
			if (stockfishEnabled && whiteToPlay != playerSideWhite) {
				return;
			}
			std::shared_ptr<Piece> p = getPieceFromCurrentPosition(sf::Vector2i(selectedPos));
			if (p && !promotePiece && !gatingPiece && !gatingKing && !gatingRook && !pieceMoving && animationFinished && !calculatingPos && !stockfish.isCalculating && !mouseMode && !generatingMoves) {
				// Side to Play == Color
				if (whiteToPlay == (p->isWhite())) {
					if (selectedPiece && variant->castlingEnabled && variant->castlingKingPiece.has(selectedPiece->id)) {
						if (!std::any_of(selectedPiece->getCaptureCastleSquares().begin(), selectedPiece->getCaptureCastleSquares().end(), [selectedPos = selectedPos](Chess::Square& square) { return square.pos == selectedPos; })) {
							selectedPiece = p;
						}
					}
					else {
						selectedPiece = p;
					}
				}
			}
		}
		else if (mouseButtonPressed->button == sf::Mouse::Button::Right)
		{
			if (timeEnabled) {
				if (whiteTimeBG.getGlobalBounds().contains(mousePos)) {
					if (controlClicked) {
						startingWhiteTime = std::max(startingWhiteTime - sf::seconds(10), sf::Time::Zero);
						setOptionChange("Initial White Time", startingWhiteTime.asSeconds());
					}
					else {
						whiteTime = std::max(whiteTime - sf::seconds(10), sf::Time::Zero);
						if (stockfish.isCalculating && !stockfish.getSettings().usesFixedTime) { stockfish.stopCalculating(); }
					}
				}
				else if (blackTimeBG.getGlobalBounds().contains(mousePos)) {
					if (controlClicked) {
						startingBlackTime = std::max(startingBlackTime - sf::seconds(10), sf::Time::Zero);
						setOptionChange("Initial Black Time", startingBlackTime.asSeconds());
					}
					else {
						blackTime = std::max(blackTime - sf::seconds(10), sf::Time::Zero);
						if (stockfish.isCalculating && !stockfish.getSettings().usesFixedTime) { stockfish.stopCalculating(); }
					}
				}
			}
		}
	}
	else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
	{
		if (keyBindsEnabled) {
			if (controlClicked) {
				if (keyPressed->code == sf::Keyboard::Key::T) {
					if (!calculatingPos && !pieceMoving && animationFinished) {
						if (timeEnabled) {
							moveSpeedI = moveSpeed;
							instantMoveI = instantMove;
							moveSpeed = 1.0f;
							instantMove = false;
							float scaleY = ((float)boardTexture.getSize().y + whiteTimeOutline.getTexture().getSize().y * 1.129f) / (float)boardTexture.getSize().y;
							startingScale *= scaleY;
							scale(scaleY);
							moveBy(0, -int(whiteTimeOutline.getTexture().getSize().y) * 1.129f / 2.0f);
							timeEnabled = false;
							if (stockfish.isCalculating && !stockfish.getSettings().usesFixedTime) { stockfish.stopCalculating(); }
						}
						else {
							moveSpeed = moveSpeedI;
							instantMove = instantMoveI;
							float scaleY = (float)boardTexture.getSize().y / ((float)boardTexture.getSize().y + whiteTimeOutline.getTexture().getSize().y * 1.129f);
							startingScale *= scaleY;
							scale(scaleY);
							moveBy(0, int(whiteTimeOutline.getTexture().getSize().y) * 1.129f / 2.0f);
							timeEnabled = true;
							float time = std::min(whiteTime.asSeconds(), blackTime.asSeconds()) + (timeIncrement.asSeconds() * 40.0f);
							if (time < 30) {
								instantMove = true;
								gameType = Chess::GameType::UltraBullet;
							}
							else if (time < 180) {
								instantMove = true;
								gameType = Chess::GameType::Bullet;
							}
							else if (time < 480) {
								moveSpeed = 1.5f;
								gameType = Chess::GameType::Blitz;
							}
							else if (time < 1500) {
								moveSpeed = 1.0f;
								gameType = Chess::GameType::Rapid;
							}
							else {
								moveSpeed = 0.8f;
								gameType = Chess::GameType::Classical;
							}
							if (stockfish.isCalculating && !stockfish.getSettings().usesFixedTime) { stockfish.stopCalculating(); }
						}
					}
				}
				if (timeEnabled) {
					if (keyPressed->code == sf::Keyboard::Key::I) {
						timeIncrement += sf::seconds(2.5f);
						if (timeIncrement < sf::Time::Zero) {
							timeIncrement = sf::Time::Zero;
						}
						setOptionChange("Time Increment", timeIncrement.asSeconds());
						if (stockfish.isCalculating && !stockfish.getSettings().usesFixedTime) { stockfish.stopCalculating(); }
					}
					else if (keyPressed->code == sf::Keyboard::Key::O) {
						timeIncrement -= sf::seconds(2.5f);
						if (timeIncrement < sf::Time::Zero) {
							timeIncrement = sf::Time::Zero;
						}
						setOptionChange("Time Increment", timeIncrement.asSeconds());
						if (stockfish.isCalculating && !stockfish.getSettings().usesFixedTime) { stockfish.stopCalculating(); }
					}
					else if (keyPressed->code == sf::Keyboard::Key::R) {
						whiteTime = startingWhiteTime;
						blackTime = startingBlackTime;
						setOptionChange("Initial Time");
						if (stockfish.isCalculating && !stockfish.getSettings().usesFixedTime) { stockfish.stopCalculating(); }
					}
					else if (keyPressed->code == sf::Keyboard::Key::S) {
						if (!calculatingPos) {
							std::swap(whiteTime, blackTime);
							setOptionChange("Switch Time");
							if (stockfish.isCalculating && !stockfish.getSettings().usesFixedTime) { stockfish.stopCalculating(); }
						}
					}
				}
			}
			else {

				if (keyPressed->code == sf::Keyboard::Key::O) {
					if (endgameVariants->count(variant->name) > 0) {
						endgamePosition = !endgamePosition;
						if (optionMode) {
							setOptionChange("Endgame Position", endgamePosition);
						}
					}
				}
				else if (keyPressed->code == sf::Keyboard::Key::T) {
					resetPosition();
					resetScale();
					if (timeEnabled) {
						moveBy(0, int(whiteTimeOutline.getTexture().getSize().y) * 1.129f / 2.0f);
					}
				}
				else if (keyPressed->code == sf::Keyboard::Key::S) {
					isPaused = !isPaused;
					if (optionMode) {
						setOptionChange("Paused", isPaused);
					}
				}
			}

			if (keyPressed->code == sf::Keyboard::Key::R) {
				if (!(!scaleMode && mouseMode && (pieceMoving || !animationFinished))) {
					scaleMode = !scaleMode;
					if (optionMode) {
						setOptionChange("Scale Mode", scaleMode);
					}
				}
			}
			else if (keyPressed->code == sf::Keyboard::Key::M) {
				if (mouseMode || (!mouseMode && (!scaleMode || (scaleMode && (!pieceMoving || (pieceMoving && !mouseMode)) && (animationFinished || (!animationFinished && !mouseMode)))) && !calculatingPos && !isPromoting && !isGating))
				{
					initialMousePos = mousePos;
					initialBoardScale = boardScale;
					mouseMode = !mouseMode;
				}
			}
			else if (keyPressed->code == sf::Keyboard::Key::P) {
				if (variantPieceTextures->find(variant->name) == variantPieceTextures->end()) {
					++pieceSet %= pieceTextures->size();
					currentPieceTextures = &pieceTextures->at(pieceSet);
					setPieceTextures();
					if (optionMode) {
						setOptionChange("Piece Set", pieceSet + 1);
					}
				}
			}
			else if (keyPressed->code == sf::Keyboard::Key::B) {
				++boardSet %= 23;
				setBoardTexture(boardSpriteSheet, boardSet);
				if (optionMode) {
					setOptionChange("Board Set", boardSet + 1);
				}
				unsigned int square = (unsigned int)(1024 / 8.0f) + 1U;
				boardHSV = RGBtoHSV(boardSprite.getTexture().copyToImage().getPixel({ square, 0 }));
			}
			else if (keyPressed->code == sf::Keyboard::Key::K) {
				if (!calculatingPos) {
					flipBoard();
				}
			}
			else if (keyPressed->code == sf::Keyboard::Key::V) {
				if (!calculatingPos && !generatingMoves) {
					if (stockfish.isCalculating) {
						stockfish.stopCalculating();
					}
					setVariant((variantIndex + 1) % variantList->size());
					if (optionMode) {
						setOptionChange("Variant", *variant);
					}
				}
			}
			else if (keyPressed->code == sf::Keyboard::Key::F) {
				if (!calculatingPos && !generatingMoves) {
					if (stockfish.isCalculating) {
						stockfish.stopCalculating();
					}
					if (variantIndex == 0) {
						if (variantList->size() > 0) {
							setVariant(variantList->size() - 1);
						}
					}
					else {
						setVariant(variantIndex - 1);
					}
					if (optionMode) {
						setOptionChange("Variant", *variant);
					}
				}
			}
			else if (keyPressed->code == sf::Keyboard::Key::D) {
				autoNewPosition = !autoNewPosition;
				setOptionChange("Auto New Game", autoNewPosition);
			}
			else if (keyPressed->code == sf::Keyboard::Key::L) {
				if (animationFinished) {
					if (!AI_Only) {
						if (stockfishEnabled && animationFinished) {
							AI_Only = true;
						}
					}
					else {
						AI_Only = false;
					}
					if (stockfishEnabled && selectedPiece) {
						if (holdingPiece) {
							selectedPiece->setPosition(getGlobalPosition(selectedPiece->getLocalPos()));
						}
						selectedPiece.reset();
					}
					if (optionMode) {
						setOptionChange("AI Only", AI_Only);
					}
				}
			}
			else if (keyPressed->code == sf::Keyboard::Key::R) {
				repeatFEN = !repeatFEN;
				if (optionMode) {
					setOptionChange("Repeat Position", repeatFEN);
				}
			}
			else if (keyPressed->code == sf::Keyboard::Key::U) {
				autoFlip = !autoFlip;
				if (optionMode) {
					setOptionChange("Auto Flip", autoFlip);
				}
			}
			else if (keyPressed->code == sf::Keyboard::Key::C) {
				if (!pieceMoving && !calculatingPos && animationFinished) {
					if (stockfish.isCalculating) {
						stockfish.stopCalculating();
					}
					position.hasChess960 = !position.hasChess960;
					stockfish.setVariant(variant->name, position.hasChess960);
					if (optionMode) {
						setOptionChange("Chess960 Enabled", position.hasChess960);
					}
				}
			}
			else if (keyPressed->code == sf::Keyboard::Key::Q) {
				optionMode = !optionMode;
				setOptionChange("Show Option Change", optionMode);
			}
			else if (keyPressed->code == sf::Keyboard::Key::H) {
				if (!calculatingPos && !generatingMoves) {
					if (stockfish.isCalculating) {
						stockfish.stopCalculating();
					}
					size_t index = std::rand() % variantList->size();
					setVariant(index);
					if (optionMode) {
						setOptionChange("Variant", *variant);
					}
				}
			}
			else if (keyPressed->code == sf::Keyboard::Key::G) {
				if (stockfishEnabled) {
					playerSideWhite = !playerSideWhite;
					setOptionChange("Player Side", playerSideWhite);
					if (stockfish.isCalculating) { stockfish.stopCalculating(); }
				}
			}
			else if (keyPressed->code == sf::Keyboard::Key::J) {
				if (!newPositionWhite.has_value()) {
					newPositionWhite = true;
				}
				else {
					if (newPositionWhite.value() == true) {
						newPositionWhite = false;
					}
					else {
						newPositionWhite = {};
					}
				}
				setOptionChange("Player Side on Restart", newPositionWhite);
			}
		}

		if (keyPressed->code == sf::Keyboard::Key::N) {
			if (!calculatingPos && !generatingMoves) {
				if (stockfish.isCalculating) { stockfish.stopCalculating(); }
				if (repeatFEN) { loadFromFEN(newPositionFEN, true); }
				else { loadFromFEN({}, true); }
			}
		}
		if (keyPressed->code == sf::Keyboard::Key::W) {
			if (!calculatingPos && !generatingMoves) {
				stockfish.stopCalculatingAndPlay();
			}
		}
		else if (keyPressed->code == sf::Keyboard::Key::A) {
			if (animationFinished) {
				stockfishEnabled = !stockfishEnabled;
				if (stockfish.isCalculating) { stockfish.stopCalculating(); }
				if (optionMode) { setOptionChange("AI Enabled", stockfishEnabled); }
				if (!stockfishEnabled) { AI_OnlyT = AI_Only; AI_Only = false; }
				else {
					AI_Only = AI_OnlyT;
					if (!startedStockfish) {
						stockfish.startStockfish();
						stockfish.setVariant(variant->name, position.hasChess960);
						startedStockfish = true;
					}
				}
				if (stockfishEnabled && selectedPiece && playerSideWhite != whiteToPlay) {
					if (holdingPiece) {
						selectedPiece->setPosition(getGlobalPosition(selectedPiece->getLocalPos()));
					}
					selectedPiece.reset();
				}
			}
		}
		else if (keyPressed->code == sf::Keyboard::Key::Left) {
			if (positionHistory.size() > 1 && !isPromoting && !isGating && !calculatingPos && (!pieceMoving || (pieceMoving && changingPosition)) && animationFinished && !generatingMoves) {
				positionChangeThreads.emplace_back(true);
			}
		}
		else if (keyPressed->code == sf::Keyboard::Key::Right) {
			if (!positionHistoryF.empty() && !isPromoting && !isGating && !calculatingPos && (!pieceMoving || (pieceMoving && changingPosition)) && animationFinished && !generatingMoves) {
				positionChangeThreads.emplace_back(false);
			}
		}
	}
}

void Board::update(sf::Vector2f& mousePos, bool isFocused, float deltaTime)
{
	if (mouseClicked && mouseMode) { mouseMode = false; }

	if (animationFinished && optionClock.getElapsedTime().asMilliseconds() > 50) {
		optionChangeText.setFillColor({ 255, 255, 255, Interpolate(optionChangeText.getFillColor().a, 0, 0.15f * deltaTime * 60) });
		optionChangeOverlay.setFillColor({ 0, 0, 0, Interpolate(optionChangeOverlay.getFillColor().a, 0, 0.15f * deltaTime * 60) });
	}

	if (animationFinished && gameEndClock.getElapsedTime().asMilliseconds() > 650) {
		gameEndText.setFillColor({ 255, 255, 255, Interpolate(gameEndText.getFillColor().a, 0, 0.15f * deltaTime * 60) });
		gameEndOverlay.setFillColor({ 0, 0, 0, Interpolate(gameEndOverlay.getFillColor().a, 0, 0.15f * deltaTime * 60) });
	}

	if (!positionChangeThreads.empty()) {
		if (positionChangeThreads.front()) {
			if (positionHistory.size() > 1 && !calculatingPos && (!pieceMoving || (pieceMoving && changingPosition)) && animationFinished && !generatingMoves) {
				if (stockfish.isCalculating) { stockfish.stopCalculating(); }
				positionChangeThreads.erase(positionChangeThreads.begin());
				setPreviousPosition();
			}
			else {
				positionChangeThreads.erase(positionChangeThreads.begin());
			}
		}
		else {
			if (stockfish.isCalculating) { stockfish.stopCalculating(); }
			if (!positionHistoryF.empty() && !calculatingPos && (!pieceMoving || (pieceMoving && changingPosition)) && animationFinished && !generatingMoves) {
				positionChangeThreads.erase(positionChangeThreads.begin());
				setNextPosition();
			}
			else {
				positionChangeThreads.erase(positionChangeThreads.begin());
			}
		}
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) && !arrowLeft) {
		if (!arrowClockL.isRunning()) { arrowClockL.restart(); }
		else if (arrowClockL.getElapsedTime().asMilliseconds() > 400) {
			arrowLeft = true;
		}
	}
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
		arrowLeft = false;
		arrowClockL.reset();
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) && !arrowRight) {
		if (!arrowClockR.isRunning()) { arrowClockR.restart(); }
		else if (arrowClockR.getElapsedTime().asMilliseconds() > 400) {
			arrowRight = true;
		}
	}
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
		arrowRight = false;
		arrowClockR.reset();
	}

	if (arrowLeft) {
		if (arrowClockL.getElapsedTime().asMilliseconds() >= 60) {
			if (positionHistory.size() > 1 && !calculatingPos && (!pieceMoving || (pieceMoving && changingPosition)) && animationFinished && !generatingMoves) {
				positionChangeThreads.emplace_back(true);
			}
			arrowClockL.restart();
		}
	}

	if (arrowRight) {
		if (arrowClockR.getElapsedTime().asMilliseconds() >= 75) {
			if (positionHistory.size() > 1 && !calculatingPos && (!pieceMoving || (pieceMoving && changingPosition)) && animationFinished && !generatingMoves) {
				positionChangeThreads.emplace_back(false);
			}
			arrowClockR.restart();
		}
	}


	// Variant Extras
	if (variant->nCheckCounting) {
		whiteCheckCount.setColor(sf::Color(255, 255, 255, 255));
		blackCheckCount.setColor(sf::Color(255, 255, 255, 255));
		blackCheckText.setFillColor(sf::Color(255, 255, 255, 255));
		whiteCheckText.setFillColor(sf::Color(255, 255, 255, 255));
		if (!holdingPiece) {
			for (auto& piece : position.pieceList) {
				if (variant->royalPieces.has(piece->id) && piece->isWhite()) {
					whiteCheckCount.setPosition({ piece->getGlobalPos().x - (boardMultiplier / 2.91f), piece->getGlobalPos().y - (boardMultiplier / 2.91f) });
					whiteCheckText.setPosition(whiteCheckCount.getPosition());
					break;
				}
			}
			for (auto& piece : position.pieceList) {
				if (variant->royalPieces.has(piece->id) && piece->isBlack()) {
					blackCheckCount.setPosition({ piece->getGlobalPos().x - (boardMultiplier / 2.91f), piece->getGlobalPos().y - (boardMultiplier / 2.91f) });
					blackCheckText.setPosition(blackCheckCount.getPosition());
					break;
				}
			}
		}
		switch (whiteChecksLeft) {
		case 5:
			whiteCheckCount.setColor(sf::Color(65, 168, 224));
			break;
		case 4:
			whiteCheckCount.setColor(sf::Color(66, 175, 137));
			break;
		case 3:
			whiteCheckCount.setColor(sf::Color(100, 173, 105));
			break;
		case 2:
			whiteCheckCount.setColor(sf::Color(242, 119, 58));
			break;
		case 1:
			whiteCheckCount.setColor(sf::Color(210, 0, 0));
			break;
		case 0:
			whiteCheckCount.setColor(sf::Color(45, 45, 45));
			break;
		default:
			whiteCheckCount.setColor(sf::Color(200, 200, 200));
			break;
		}
		whiteCheckText.setString(std::to_string(whiteChecksLeft));
		whiteCheckText.setOrigin({ whiteCheckText.getLocalBounds().position.x + whiteCheckText.getLocalBounds().size.x / 2.0f, whiteCheckText.getLocalBounds().position.y + whiteCheckText.getLocalBounds().size.y / 2.0f });
		if (whiteChecksLeft >= 100) {
			whiteCheckText.setCharacterSize(18U);
		}
		else if (whiteChecksLeft >= 10) {
			whiteCheckText.setCharacterSize(38U);
		}
		else {
			whiteCheckText.setCharacterSize(43U);
		}
		switch (blackChecksLeft) {
		case 5:
			blackCheckCount.setColor(sf::Color(65, 168, 224));
			break;
		case 4:
			blackCheckCount.setColor(sf::Color(66, 175, 137));
			break;
		case 3:
			blackCheckCount.setColor(sf::Color(100, 173, 105));
			break;
		case 2:
			blackCheckCount.setColor(sf::Color(242, 119, 58));
			break;
		case 1:
			blackCheckCount.setColor(sf::Color(210, 0, 0));
			break;
		case 0:
			blackCheckCount.setColor(sf::Color(45, 45, 45));
			break;
		default:
			blackCheckCount.setColor(sf::Color(200, 200, 200));
			break;
		}
		blackCheckText.setString(std::to_string(blackChecksLeft));
		blackCheckText.setOrigin({ blackCheckText.getLocalBounds().position.x + blackCheckText.getLocalBounds().size.x / 2.0f, blackCheckText.getLocalBounds().position.y + blackCheckText.getLocalBounds().size.y / 2.0f });
		if (blackChecksLeft >= 100) {
			blackCheckText.setCharacterSize(18U);
		}
		else if (blackChecksLeft >= 10) {
			blackCheckText.setCharacterSize(38U);
		}
		else {
			blackCheckText.setCharacterSize(43U);
		}
	}
	else {
		whiteCheckCount.setColor(sf::Color(255, 255, 255, 0));
		blackCheckCount.setColor(sf::Color(255, 255, 255, 0));
		blackCheckText.setFillColor(sf::Color(255, 255, 255, 0));
		whiteCheckText.setFillColor(sf::Color(255, 255, 255, 0));
	}

	// Board Animation
	if (std::any_of(position.pieceList.begin(), position.pieceList.end(), [](std::shared_ptr<Piece>& piece) { return piece->hasAnimationTarget(); })) {
		animationFinished = false;
		if (animationClock.getElapsedTime().asMilliseconds() > 250) {
			for (auto& piece : position.pieceList) {
				if (piece) { piece->updatePosition(moveSpeed, deltaTime); }
			}
		}
	}
	else { animationFinished = true; }

	if (!position.gameEnded) {
		if (stockfish.movePlayed) {
			stockfish.movePlayed = false;
			if (stockfishEnabled && !stockfish.getMove().empty()) {
				std::string m = stockfish.getMove();
				playMove(m, instantMove);
			}
			stockfish.resetMove();
		}
	}
	if (std::any_of(position.pieceList.begin(), position.pieceList.end(), [](std::shared_ptr<Piece>& piece) { return piece->hasTarget(); })) {
		pieceMoving = true;
	}
	else { pieceMoving = false; }

	if (!calculatingPos) {
		// Stockfish Move
		if (stockfishEnabled && (AI_Only || (!AI_Only && whiteToPlay != playerSideWhite)) && animationFinished && !pieceMoving && !playingMove && !generatingMoves && !stockfish.isCalculating && delayClock.getElapsedTime().asMilliseconds() > 100) {
			if (timeEnabled && !isPaused) {
				stockfish.isCalculating = true;
				std::thread moveT(&Stockfish::getBestMoveT, &stockfish, startingFEN, moves, whiteTime, blackTime, timeIncrement);
				moveT.detach();
			}
			else {
				stockfish.isCalculating = true;
				std::thread moveT(&Stockfish::getBestMove, &stockfish, startingFEN, moves);
				moveT.detach();
			}
		}

		// Movement
		if (!stockfish.isCalculating) {
			if (!changingPosition) {
				if (castleKing && castleRook) {
					if (castleKing->getTarget() != castleKing->getGlobalPos() || castleRook->getTarget() != castleRook->getGlobalPos()) {
						castleKing->updatePosition(moveSpeed, deltaTime);
						castleRook->updatePosition(moveSpeed, deltaTime);
					}
					else {
						calculatingPos = true;
						std::thread postMoveF(&Board::postMove, this, pieceVector{ castleKing, castleRook }, false);
						castleKing.reset();
						castleRook.reset();
						postMoveF.join();
					}
				}
				else if (gatingKing && gatingRook && gatingKing->hasTarget() && gatingRook->hasTarget()) {
					if (gatingKing->getTarget() != gatingKing->getGlobalPos() || gatingRook->getTarget() != gatingRook->getGlobalPos()) {
						gatingKing->updatePosition(moveSpeed, deltaTime);
						gatingRook->updatePosition(moveSpeed, deltaTime);
					}
					else {
						if (cancelGating) {
							gatingKing->setTarget({});
							gatingRook->setTarget({});
							gatingKing.reset();
							gatingRook.reset();
							cancelGating = false;
						}
						else {
							isGating = true;
							gatingKing->setTarget({});
							gatingRook->setTarget({});
							setupGatingSprites();
						}
					}
				}
				else {
					for (auto& piece : position.pieceList) {
						if (piece && piece->hasTarget()) {
							if (piece->getGlobalPos() == piece->getTarget()) {
								if (shouldPostMove) {
									calculatingPos = true;
									std::thread postMoveF(&Board::postMove, this, pieceVector{ piece }, false);
									postMoveF.join();
								}
								else {
									piece->setTarget({});
									shouldPostMove = true;
									if (promotePiece) {
										isPromoting = true;
										setupPromoteSprites();
									}
									else if (gatingPiece) {
										isGating = true;
										setupGatingSprites();
									}
								}
								break;
							}
							else {
								if (capturePiece) {
									piece->updatePosition(moveSpeed, captureThreshold, deltaTime);
									if (std::fmax(std::abs(piece->getGlobalPos().x - piece->getTarget().x), std::abs(piece->getGlobalPos().y - piece->getTarget().y)) < 5.0f) {
										setSpriteVisible(ghostSprite, false);
										capturePiece->setVisible(false);
									}
								}
								else {
									piece->updatePosition(moveSpeed, deltaTime);
								}
							}
						}
					}
				}
			}
			else if (changingPosition) {
				if (castleKing && castleRook) {
					if (castleKing->getTarget() != castleKing->getGlobalPos() || castleRook->getTarget() != castleRook->getGlobalPos()) {
						castleKing->updatePosition(moveSpeed, deltaTime);
						castleRook->updatePosition(moveSpeed, deltaTime);
					}
					else {
						castleKing->setTarget({});
						castleRook->setTarget({});
						castleKing.reset();
						castleRook.reset();
						if (forwardMove) {
							loadFromPosition(positionHistoryF.front());
							if (!positionHistoryF.empty()) {
								positionHistoryF.erase(positionHistoryF.begin());
							}
							auto pos = savePosition();
							if (allPositionsPlayed.count(pos) > 0) {
								allPositionsPlayed.find(pos)->second++;
							}
							else {
								allPositionsPlayed.emplace(pos, 1);
							}
							if (playerSideWhite != whiteToPlay) {
								if (stockfishEnabled) {
									ai = true;
								}
								stockfishEnabled = false;
							}
							else if (ai) {
								stockfishEnabled = true;
								ai = false;
							}
							forwardMove = false;
						}
						setSpriteVisible(ghostSprite, false);
						changingPosition = false;
					}
				}
				else {
					for (auto& piece : position.pieceList) {
						if (piece && piece->hasTarget()) {
							if (piece->getGlobalPos() == piece->getTarget()) {
								sf::Vector2f pieceTarget = piece->getTarget();
								piece->setTarget({});
								if (forwardMove) {
									if (variant->atomicExplosions && atomicExplosionEffect && getPieceFromCurrentPosition(getLocalPosition(pieceTarget)) != nullptr) {
										sf::Vector2i capturePos = getLocalPosition(pieceTarget);
										for (int x = -1; x <= 1; x++) {
											for (int y = -1; y <= 1; y++) {
												sf::Vector2i newPos = { capturePos.x + x, capturePos.y + y };
												if (Chess::isValidSquare(newPos, variant->boardSize)) {
													sf::RectangleShape atomicRect{ {boardMultiplier, boardMultiplier } };
													atomicRect.setOrigin(atomicRect.getLocalBounds().getCenter());
													atomicRect.setPosition(getGlobalPosition(newPos));
													atomicRect.setFillColor(sf::Color{ 255, 255, 255, 150 });
													captureObjects.push_back(std::move(atomicRect));
												}
											}
										}
										atomicClock.restart();
									}
									loadFromPosition(positionHistoryF.front());
									if (!positionHistoryF.empty()) {
										positionHistoryF.erase(positionHistoryF.begin());
									}
									auto pos = savePosition();
									if (allPositionsPlayed.count(pos) > 0) {
										allPositionsPlayed.find(pos)->second++;
									}
									else {
										allPositionsPlayed.emplace(pos, 1);
									}
									if (playerSideWhite != whiteToPlay) {
										if (stockfishEnabled) {
											ai = true;
										}
										stockfishEnabled = false;
									}
									else if (ai) {
										stockfishEnabled = true;
										ai = false;
									}
									forwardMove = false;
								}
								setSpriteVisible(ghostSprite, false);
								changingPosition = false;
								break;
							}
							else {
								if (!instantMove) {
									if (forwardMove) {
										std::shared_ptr<Piece> capture = getPieceFromCurrentPosition(getLocalPosition(piece->getTarget()));
										if (capture) {
											piece->updatePosition(moveSpeed, captureThreshold, deltaTime);
											if (std::fmax(std::abs(piece->getGlobalPos().x - piece->getTarget().x), std::abs(piece->getGlobalPos().y - piece->getTarget().y)) < 5.0f) {
												setSpriteVisible(ghostSprite, false);
												capture->setVisible(false);
											}
										}
										else {
											piece->updatePosition(moveSpeed, deltaTime);
										}
									}
									else {
										piece->updatePosition(moveSpeed, deltaTime);
									}
								}
								else {
									piece->setPosition(piece->getTarget());
								}
							}
						}
					}
				}
			}
		}

		// Click Detection
		if (mouseClicked && !pieceMoving && animationFinished && !mouseMode && !holdingPiece && !stockfish.isCalculating) {
			if (selectedPiece) {
				if (selectedPiece->hasMoveIntersecting(mousePos) && !position.gameEnded) {
					const Chess::Square& square = selectedPiece->getMoveIntersecting(mousePos);
					std::string pieceMove = square.moveString;
					if (!pieceMove.empty()) {
						const auto& currentPromotionRegion = selectedPiece->isWhite() ? variant->whitePromotionRegion : variant->blackPromotionRegion;
						const auto& currentPromotedPieces = selectedPiece->isWhite() ? variant->whitePromotePieces : variant->blackPromotePieces;
						Chess::PieceType* validTypes = nullptr;
						for (auto& sq : selectedPiece->getMoveSquares()) {
							if (sq.promoteSquare && sq.pos == square.pos) {
								validTypes = &sq.validPromotionTypes;
								break;
							}
						}
						if (!validTypes) {
							for (auto& sq : selectedPiece->getCaptureSquares()) {
								if (sq.promoteSquare && sq.pos == square.pos) {
									validTypes = &sq.validPromotionTypes;
									break;
								}
							}
						}
						auto& currentDropPieces = whiteToPlay ? whiteDropPieces : blackDropPieces;
						size_t dropCount = 0;
						for (auto& piece : currentDropPieces) {
							if (piece.count > 0 && !variant->pawnPieces.has(piece.id)) {
								dropCount++;
							}
						}
						bool isPiecePromotion = (variant->promotedPieceTypes.find(selectedPiece->id) != variant->promotedPieceTypes.end()) || (variant->pieceDemotion && selectedPiece->promotedPieceType && selectedPiece->demotedID != ' ');
						bool promoteMove = validTypes && ((variant->promotionPawnPieces.has(selectedPiece->id) || variant->pawnPieces.has(selectedPiece->id) || isPiecePromotion)
							&& (currentPromotionRegion.contains(square.pos) || (isPiecePromotion && currentPromotionRegion.contains(selectedPiece->getLocalPos()))) && validTypes->count() > 1);
						bool gatingMove = variant->gating && variant->seirawanGating && dropCount > 0 && !selectedPiece->hasMoved &&
							selectedPiece->getLocalPos().y == (selectedPiece->isWhite() ? 1 : variant->boardSize.y);
						playMove(pieceMove, instantMove || gatingMove, promoteMove || gatingMove);
					}
				}
				else if (selectedPiece->contains(mousePos)) { holdingPiece = true; }
			}
			if (getPieceFromCurrentPosition(sf::Vector2i(selectedPos)) == nullptr) {
				selectedPiece.reset();
			}
		}
		if (selectedPiece) {
			selectedPieceBackground.setPosition(getGlobalPosition(selectedPiece->getLocalPos()));
			if (holdingPiece && !sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
				holdingPiece = false;
				if (selectedPiece->hasMoveIntersecting(mousePos) && !position.gameEnded) {
					const Chess::Square& square = selectedPiece->getMoveIntersecting(mousePos);
					std::string pieceMove = square.moveString;
					if (!pieceMove.empty()) {
						const auto& currentPromotionRegion = selectedPiece->isWhite() ? variant->whitePromotionRegion : variant->blackPromotionRegion;
						const auto& currentPromotedPieces = selectedPiece->isWhite() ? variant->whitePromotePieces : variant->blackPromotePieces;
						Chess::PieceType* validTypes = nullptr;
						for (auto& sq : selectedPiece->getMoveSquares()) {
							if (sq.promoteSquare && sq.pos == square.pos) {
								validTypes = &sq.validPromotionTypes;
								break;
							}
						}
						if (!validTypes) {
							for (auto& sq : selectedPiece->getCaptureSquares()) {
								if (sq.promoteSquare && sq.pos == square.pos) {
									validTypes = &sq.validPromotionTypes;
									break;
								}
							}
						}
						auto& currentDropPieces = whiteToPlay ? whiteDropPieces : blackDropPieces;
						size_t dropCount = 0;
						for (auto& piece : currentDropPieces) {
							if (piece.count > 0 && !variant->pawnPieces.has(piece.id)) {
								dropCount++;
							}
						}
						bool isPiecePromotion = (variant->promotedPieceTypes.find(selectedPiece->id) != variant->promotedPieceTypes.end()) || (variant->pieceDemotion && selectedPiece->promotedPieceType && selectedPiece->demotedID != ' ');
						bool promoteMove = validTypes && ((variant->promotionPawnPieces.has(selectedPiece->id) || variant->pawnPieces.has(selectedPiece->id) || isPiecePromotion)
							&& (currentPromotionRegion.contains(square.pos) || (isPiecePromotion && currentPromotionRegion.contains(selectedPiece->getLocalPos()))) && validTypes->count() > 1);
						bool gatingMove = variant->gating && variant->seirawanGating && dropCount > 0 && !selectedPiece->hasMoved &&
							selectedPiece->getLocalPos().y == (selectedPiece->isWhite() ? 1 : variant->boardSize.y);
						playMove(pieceMove, true, promoteMove || gatingMove);
						setSpriteVisible(ghostSprite, false);
					}
				}
				else {
					selectedPiece->setPosition(getGlobalPosition(selectedPiece->getLocalPos()));
					if (!selectedPieceBackground.getGlobalBounds().contains(selectedPiece->getGlobalPos())) {
						selectedPiece.reset();
					}
				}
			}
		}
	}

	// Sprite Squares
	if (!pieceMoving && selectedPiece && !isPromoting && !isGating && !calculatingPos && !mouseMode) {
		selectedPiece->updateSprites(*boardTextures, mousePos, mouseSelecting);
	}

	if (selectedPiece) {
		if (holdingPiece) {
			ghostSprite.setTexture(selectedPiece->getTexture(), true);
			ghostSprite.setPosition(getGlobalPosition(selectedPiece->getLocalPos()));
			setSpriteVisible(ghostSprite, true, 75);
			selectedPiece->setPosition(sf::Vector2f(mousePos));
		}
		else {
			setSpriteVisible(ghostSprite, false);
		}
	}

	// Promotion
	if (variant->hasPromotion) {
		if (isPromoting && promotePiece && !calculatingPos && !stockfish.isCalculating) {
			for (auto& promotionPiece : promotePieces) {
				sf::Sprite& sprite = promotionPiece.sprite;
				sf::Sprite& backgroundSprite = promotionPiece.backgroundSprite;
				if (backgroundSprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
					mouseSelecting = true;
					backgroundSprite.setTexture(boardTextures->at(7));
					backgroundSprite.setColor(promotionSquareSelectedColor);
					sprite.setScale(Interpolate(sprite.getScale(), promotionPiece.initialScale + sf::Vector2f{ 0.035f, 0.035f }, 0.5f * deltaTime * 60, 0.01f));
				}
				else {
					backgroundSprite.setTexture(boardTextures->at(6));
					backgroundSprite.setColor(promotionSquareColor);
					sprite.setScale(Interpolate(sprite.getScale(), promotionPiece.initialScale, 0.5f * deltaTime * 60, 0.01f));
				}
			}
			if (mouseClicked) {
				bool intersects = false;
				for (auto& promotionPiece : promotePieces) {
					if (promotionPiece.backgroundSprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
						intersects = true;
						auto find = variant->promotedPieceTypes.find(promotePiece->id);
						sf::Vector2i localPos = getLocalPosition(promotePiece->getGlobalPos());
						std::string moveString;
						if (find != variant->promotedPieceTypes.end() && find->second == promotionPiece.id) {
							moveString = Chess::convertPositiontoNotation(promotePiece->getLocalPos()) + Chess::convertPositiontoNotation(localPos) + '+';
						}
						else if (promotePiece->promotedPieceType && promotePiece->demotedID == promotionPiece.id) {
							moveString = Chess::convertPositiontoNotation(promotePiece->getLocalPos()) + Chess::convertPositiontoNotation(localPos) + '-';
						}
						else {
							if (promotePiece->id == promotionPiece.id) {
								moveString = Chess::convertPositiontoNotation(promotePiece->getLocalPos()) + Chess::convertPositiontoNotation(localPos);
							}
							else {
								moveString = Chess::convertPositiontoNotation(promotePiece->getLocalPos()) + Chess::convertPositiontoNotation(localPos) + promotionPiece.id;
							}
						}
						promotePiece.reset();
						promotePieces.clear();
						isPromoting = false;
						playMove(moveString, true);
					}
				}
				if (!intersects) {
					shouldPostMove = false;
					isPromoting = false;
					promotePieces.clear();
					if (capturePiece) {
						setSpriteVisible(ghostSprite, false);
						capturePiece->setVisible(true);
						capturePiece.reset();
					}
					promotePiece->setTarget(getGlobalPosition(promotePiece->getLocalPos()));
					promotePiece.reset();
				}
			}
		}
	}
	if (variant->gating && variant->seirawanGating) {
		if (isGating && gatingKing && gatingRook && !calculatingPos && !stockfish.isCalculating) {
			for (auto& gatePiece : gatingKingPieces) {
				sf::Sprite& sprite = gatePiece.sprite;
				sf::Sprite& backgroundSprite = gatePiece.backgroundSprite;
				if (backgroundSprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
					mouseSelecting = true;
					backgroundSprite.setTexture(boardTextures->at(7));
					backgroundSprite.setColor(promotionSquareSelectedColor);
					sprite.setScale(Interpolate(sprite.getScale(), gatePiece.initialScale + sf::Vector2f{ 0.035f, 0.035f }, 0.5f * deltaTime * 60, 0.01f));
				}
				else {
					backgroundSprite.setTexture(boardTextures->at(6));
					backgroundSprite.setColor(promotionSquareColor);
					sprite.setScale(Interpolate(sprite.getScale(), gatePiece.initialScale, 0.5f * deltaTime * 60, 0.01f));

				}
			}
			for (auto& gatePiece : gatingRookPieces) {
				sf::Sprite& sprite = gatePiece.sprite;
				sf::Sprite& backgroundSprite = gatePiece.backgroundSprite;
				if (backgroundSprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
					mouseSelecting = true;
					backgroundSprite.setTexture(boardTextures->at(7));
					backgroundSprite.setColor(promotionSquareSelectedColor);
					sprite.setScale(Interpolate(sprite.getScale(), gatePiece.initialScale + sf::Vector2f{ 0.035f, 0.035f }, 0.5f * deltaTime * 60, 0.01f));
				}
				else {
					backgroundSprite.setTexture(boardTextures->at(6));
					backgroundSprite.setColor(promotionSquareColor);
					sprite.setScale(Interpolate(sprite.getScale(), gatePiece.initialScale, 0.5f * deltaTime * 60, 0.01f));
				}
			}
			if (mouseClicked) {
				bool intersects = false;
				for (auto& gatePiece : gatingKingPieces) {
					if (gatePiece.backgroundSprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
						intersects = true;
						sf::Vector2i localPos = getLocalPosition(gatingKing->getGlobalPos());
						std::string moveString;
						if (gatingKing->id == gatePiece.id && gatePiece.self) {
							if (position.hasChess960) {
								moveString = Chess::convertPositiontoNotation(gatingKing->getLocalPos()) + Chess::convertPositiontoNotation(gatingRook->getLocalPos());
							}
							else {
								moveString = Chess::convertPositiontoNotation(gatingKing->getLocalPos()) + Chess::convertPositiontoNotation(localPos);
							}
						}
						else {
							if (position.hasChess960) {
								moveString = Chess::convertPositiontoNotation(gatingKing->getLocalPos()) + Chess::convertPositiontoNotation(gatingRook->getLocalPos()) + gatePiece.id;
							}
							else {
								moveString = Chess::convertPositiontoNotation(gatingKing->getLocalPos()) + Chess::convertPositiontoNotation(localPos) + gatePiece.id;
							}
						}
						gatingKing.reset();
						gatingRook.reset();
						gatingRookPieces.clear();
						gatingKingPieces.clear();
						isGating = false;
						playMove(moveString, true);
					}
				}
				for (auto& gatePiece : gatingRookPieces) {
					if (gatePiece.backgroundSprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
						intersects = true;
						sf::Vector2i localPos = getLocalPosition(gatingRook->getGlobalPos());
						std::string moveString;
						if (gatingRook->id == gatePiece.id && gatePiece.self) {
							if (position.hasChess960) {
								moveString = Chess::convertPositiontoNotation(gatingKing->getLocalPos()) + Chess::convertPositiontoNotation(gatingRook->getLocalPos());
							}
							else {
								moveString = Chess::convertPositiontoNotation(gatingKing->getLocalPos()) + Chess::convertPositiontoNotation(getLocalPosition(gatingKing->getGlobalPos()));
							}
						}
						else {
							moveString = Chess::convertPositiontoNotation(gatingRook->getLocalPos()) + Chess::convertPositiontoNotation(gatingKing->getLocalPos()) + gatePiece.id;
						}
						gatingKing.reset();
						gatingRook.reset();
						gatingRookPieces.clear();
						gatingKingPieces.clear();
						isGating = false;
						playMove(moveString, true);
					}
				}
				if (!intersects) {
					isGating = false;
					gatingRookPieces.clear();
					gatingKingPieces.clear();
					if (capturePiece) {
						setSpriteVisible(ghostSprite, false);
						capturePiece->setVisible(true);
						capturePiece.reset();
					}
					gatingRook->setTarget(getGlobalPosition(gatingRook->getLocalPos()));
					gatingKing->setTarget(getGlobalPosition(gatingKing->getLocalPos()));
					cancelGating = true;
				}
			}
		}
		else if (isGating && gatingPiece && !calculatingPos && !stockfish.isCalculating) {
			for (auto& gatePiece : gatingPieces) {
				sf::Sprite& sprite = gatePiece.sprite;
				sf::Sprite& backgroundSprite = gatePiece.backgroundSprite;
				if (backgroundSprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
					mouseSelecting = true;
					backgroundSprite.setTexture(boardTextures->at(7));
					backgroundSprite.setColor(promotionSquareSelectedColor);
					sprite.setScale(Interpolate(sprite.getScale(), gatePiece.initialScale + sf::Vector2f{ 0.035f, 0.035f }, 0.5f * deltaTime * 60, 0.01f));
				}
				else {
					backgroundSprite.setTexture(boardTextures->at(6));
					backgroundSprite.setColor(promotionSquareColor);
					sprite.setScale(Interpolate(sprite.getScale(), gatePiece.initialScale, 0.5f * deltaTime * 60, 0.01f));
				}
			}
			if (mouseClicked) {
				bool intersects = false;
				for (auto& gatePiece : gatingPieces) {
					if (gatePiece.backgroundSprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
						intersects = true;
						sf::Vector2i localPos = getLocalPosition(gatingPiece->getGlobalPos());
						std::string moveString;
						if (gatingPiece->id == gatePiece.id && gatePiece.self) {
							moveString = Chess::convertPositiontoNotation(gatingPiece->getLocalPos()) + Chess::convertPositiontoNotation(localPos);
						}
						else {
							moveString = Chess::convertPositiontoNotation(gatingPiece->getLocalPos()) + Chess::convertPositiontoNotation(localPos) + gatePiece.id;
						}
						gatingPiece.reset();
						gatingPieces.clear();
						isGating = false;
						playMove(moveString, true);
					}
				}
				if (!intersects) {
					shouldPostMove = false;
					isGating = false;
					gatingPieces.clear();
					if (capturePiece) {
						setSpriteVisible(ghostSprite, false);
						capturePiece->setVisible(true);
						capturePiece.reset();
					}
					gatingPiece->setTarget(getGlobalPosition(gatingPiece->getLocalPos()));
					gatingPiece.reset();
				}
			}
		}
	}

	// Drop Pieces
	if (!calculatingPos && !stockfish.isCalculating) {
		std::vector<Chess::DropPiece>& currentDropPieces = whiteToPlay ? whiteDropPieces : blackDropPieces;
		Chess::PColor currentColor = whiteToPlay ? Chess::PColor::White : Chess::PColor::Black;
		for (auto& piece : currentDropPieces) {
			if (piece.mouseSelecting(mousePos) && !holdingDropPiece) {
				mouseSelecting = true;
				if (mouseClicked) {
					selectedDropPiece.set(getTextureFromID(piece.id, currentColor), piece.dropSquares, piece.id);
					holdingDropPiece = true;
				}
				piece.background.setColor({ 255, 255, 255, Interpolate(piece.background.getColor().a, 100, 0.3f * deltaTime * 60) });
			}
			else {
				piece.background.setColor({ 255, 255, 255, Interpolate(piece.background.getColor().a, 0, 0.3f * deltaTime * 60) });
			}
		}
		if (holdingDropPiece) {
			if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
				holdingDropPiece = false;
				if (selectedDropPiece.sDropSquares) {
					for (auto& square : *selectedDropPiece.sDropSquares) {
						sf::Sprite& sprite = *square.sprite;
						if (selectedDropPiece.sprite.getGlobalBounds().contains(sprite.getPosition())) {
							playMove(square.moveString, false);
						}
					}
				}
			}
			else {
				mouseSelecting = true;
				selectedDropPiece.sprite.setPosition(mousePos);
				if (selectedDropPiece.sDropSquares) {
					for (auto& square : *selectedDropPiece.sDropSquares) {
						sf::Sprite& sprite = *square.sprite;
						if (selectedDropPiece.sprite.getGlobalBounds().contains(sprite.getPosition())) {
							square.setTexture(boardTextures->at(4));
						}
						else {
							square.setTexture(boardTextures->at(0));
						}
					}
				}
			}
		}
	}


	// Capture Effect (Atomic)
	if (atomicClock.isRunning()) {
		if (atomicClock.getElapsedTime().asMilliseconds() >= 150) {
			captureObjects.clear();
			atomicClock.reset();
			atomicClock.stop();
		}
		else if (atomicClock.getElapsedTime().asMilliseconds() >= 75) {
			for (auto& obj : captureObjects) {
				obj.setFillColor(sf::Color{ 255, 0, 0, 130 });
			}
		}
	}

	// Time
	if (timeEnabled) {
		sf::Time* whiteT = &whiteTime;
		sf::Time* blackT = &blackTime;
		if (AI_Time) {
			if (playerSideWhite) {
				whiteT = &playerTime;
				blackT = &AITime;
			}
			else {
				whiteT = &AITime;
				blackT = &playerTime;
			}
		}
		if ((delayClock.isRunning() && delayClock.getElapsedTime().asMilliseconds() < 100) || isPaused) {
			timeClock.reset();
		}
		else if (!timeClock.isRunning() && allPositionsPlayed.size() > 2 && animationFinished && !pieceMoving && !isPaused) {
			timeClock.start();
		}
		if (timeClock.isRunning() && animationFinished && !pieceMoving && !isPaused && !generatingMoves && !position.gameEnded) {
			if (whiteToPlay) {
				sf::Time temp = *whiteT;
				*whiteT -= timeClock.restart();
				if (temp.asSeconds() >= 10 && whiteT->asSeconds() < 10 && !AI_Only && (!stockfishEnabled || stockfishEnabled && playerSideWhite == whiteToPlay)) {
					lowTimeSound.play();
				}
				if (*whiteT < sf::Time::Zero) { *whiteT = sf::Time::Zero; }
			}
			else {
				sf::Time temp = *blackT;
				*blackT -= timeClock.restart();
				if (temp.asSeconds() >= 10 && blackT->asSeconds() < 10 && !AI_Only && (!stockfishEnabled || stockfishEnabled && playerSideWhite == whiteToPlay)) {
					lowTimeSound.play();
				}
				if (*blackT < sf::Time::Zero) { *blackT = sf::Time::Zero; }
			}
			if (*whiteT == sf::Time::Zero || *blackT == sf::Time::Zero) {
				position.gameEnded = true;
				std::string sideToPlayString = *whiteT == sf::Time::Zero ? "White " : "Black ";
				setGameEndText(sideToPlayString + "Lost On Time");
			}
		}

		std::string minutes = std::to_string((int)std::fmodf(std::floor(whiteT->asSeconds() / 60.0f), 60.0f));
		std::string seconds = std::to_string((int)std::fmodf(whiteT->asSeconds(), 60.0f));
		std::string hours = std::to_string((int)std::floor(whiteT->asSeconds() / 3600.0f));
		if (minutes.size() == 1) { minutes.insert(0, "0"); }
		if (seconds.size() == 1) { seconds.insert(0, "0"); }
		if (hours.size() == 1) { hours.insert(0, "0"); }

		if (whiteT->asSeconds() >= 3600) {
			whiteTimeText.setString(hours + ":" + minutes + ":" + seconds);
			if (whiteTimeBG.getScale().x != boardScale * 1.632f) {
				whiteTimeBG.setScale({ boardScale * 1.632f, boardScale * 1.02f });
				whiteTimeOutline.setScale({ boardScale * 1.715f, boardScale * 1.129f });
			}
		}
		else {
			whiteTimeText.setString(minutes + ":" + seconds);
			if (whiteTimeBG.getScale().x != boardScale * 1.02f) {
				whiteTimeBG.setScale({ boardScale * 1.02f, boardScale * 1.02f });
				whiteTimeOutline.setScale({ boardScale * 1.11f, boardScale * 1.129f });
			}
		}
		if (millisecondsCondition(*whiteT)) {
			int millis = whiteT->asMilliseconds() % 1000;
			millis = std::floor(millis / 100.0f);
			std::string milliseconds = std::to_string(millis);
			if (whiteT->asSeconds() >= 3600) {
				whiteTimeText.setString(hours + ":" + minutes + ":" + seconds + "." + milliseconds);
				if (whiteTimeBG.getScale().x != boardScale * 1.632f) {
					whiteTimeBG.setScale({ boardScale * 1.632f, boardScale * 1.02f });
					whiteTimeOutline.setScale({ boardScale * 1.715f, boardScale * 1.129f });
				}
			}
			else {
				whiteTimeText.setString(minutes + ":" + seconds + "." + milliseconds);
				if (whiteTimeBG.getScale().x != boardScale * 1.25f) {
					whiteTimeBG.setScale({ boardScale * 1.25f, boardScale * 1.02f });
					whiteTimeOutline.setScale({ boardScale * 1.36f, boardScale * 1.129f });
				}
			}
		}
		whiteTimeText.setOrigin({ whiteTimeText.getLocalBounds().position.x + (whiteTimeText.getLocalBounds().size.x / 2.0f), whiteTimeText.getLocalBounds().position.y + (whiteTimeText.getLocalBounds().size.y / 2.0f) });

		minutes = std::to_string((int)std::fmodf(std::floor(blackT->asSeconds() / 60.0f), 60.0f));
		seconds = std::to_string((int)std::fmodf(blackT->asSeconds(), 60.0f));
		hours = std::to_string((int)std::floor(blackT->asSeconds() / 3600.0f));
		if (minutes.size() == 1) { minutes.insert(0, "0"); }
		if (seconds.size() == 1) { seconds.insert(0, "0"); }
		if (hours.size() == 1) { hours.insert(0, "0"); }

		if (blackT->asSeconds() >= 3600) {
			blackTimeText.setString(hours + ":" + minutes + ":" + seconds);
			if (blackTimeBG.getScale().x != boardScale * 1.632f) {
				blackTimeBG.setScale({ boardScale * 1.632f, boardScale * 1.02f });
				blackTimeOutline.setScale({ boardScale * 1.715f, boardScale * 1.129f });
			}
		}
		else {
			blackTimeText.setString(minutes + ":" + seconds);
			if (blackTimeBG.getScale().x != boardScale * 1.02f) {
				blackTimeBG.setScale({ boardScale * 1.02f, boardScale * 1.02f });
				blackTimeOutline.setScale({ boardScale * 1.11f, boardScale * 1.129f });
			}
		}
		if (millisecondsCondition(*blackT)) {
			int millis = blackTime.asMilliseconds() % 1000;
			millis = std::floor(millis / 100.0f);
			std::string milliseconds = std::to_string(millis);
			if (blackTime.asSeconds() >= 3600) {
				blackTimeText.setString(hours + ":" + minutes + ":" + seconds + "." + milliseconds);
				if (blackTimeBG.getScale().x != boardScale * 1.632f) {
					blackTimeBG.setScale({ boardScale * 1.632f, boardScale * 1.02f });
					blackTimeOutline.setScale({ boardScale * 1.715f, boardScale * 1.129f });
				}
			}
			else {
				blackTimeText.setString(minutes + ":" + seconds + "." + milliseconds);
				if (blackTimeBG.getScale().x != boardScale * 1.25f) {
					blackTimeBG.setScale({ boardScale * 1.25f, boardScale * 1.02f });
					blackTimeOutline.setScale({ boardScale * 1.36f, boardScale * 1.129f });
				}
			}
		}

		blackTimeText.setOrigin({ blackTimeText.getLocalBounds().position.x + (blackTimeText.getLocalBounds().size.x / 2.0f), blackTimeText.getLocalBounds().position.y + (blackTimeText.getLocalBounds().size.y / 2.0f) });

		if (whiteToPlay) {
			if (whiteT->asSeconds() < 10) {
				whiteTimeBG.setColor(blendColors(setHueSat(sf::Color(235, 230, 239), boardHSV), sf::Color(255, 0, 0, 255), 0.45f));
				whiteTimeOutline.setColor(blendColors(setHueSat(sf::Color(176, 158, 191), boardHSV), sf::Color(255, 0, 0, 255), 0.4f));
			}
			else {
				whiteTimeBG.setColor(setHueSat(sf::Color(235, 230, 239), boardHSV));
				whiteTimeOutline.setColor(setHueSat(sf::Color(176, 158, 191), boardHSV));
			}
			blackTimeBG.setColor(setHueSat(sf::Color(16, 9, 23), boardHSV));
			blackTimeOutline.setColor(setHueSat(sf::Color(74, 67, 78), boardHSV));
			blackTimeText.setFillColor(sf::Color(255, 255, 255));
			whiteTimeText.setFillColor(setHueSat(sf::Color(22, 8, 33), boardHSV));
		}
		else {
			if (blackT->asSeconds() < 10) {
				blackTimeBG.setColor(blendColors(setHueSat(sf::Color(235, 230, 239), boardHSV), sf::Color(255, 0, 0, 255), 0.45f));
				blackTimeOutline.setColor(blendColors(setHueSat(sf::Color(176, 158, 191), boardHSV), sf::Color(255, 0, 0, 255), 0.4f));
			}
			else {
				blackTimeBG.setColor(setHueSat(sf::Color(235, 230, 239), boardHSV));
				blackTimeOutline.setColor(setHueSat(sf::Color(176, 158, 191), boardHSV));
			}
			whiteTimeBG.setColor(setHueSat(sf::Color(16, 9, 23), boardHSV));
			whiteTimeOutline.setColor(setHueSat(sf::Color(74, 67, 78), boardHSV));
			whiteTimeText.setFillColor(sf::Color(255, 255, 255));
			blackTimeText.setFillColor(setHueSat(sf::Color(22, 8, 33), boardHSV));
		}
	}

	if (position.gameEnded && autoNewPosition) {
		if (animationFinished && !pieceMoving && !calculatingPos && !stockfish.isCalculating) {
			if (repeatFEN) { loadFromFEN(newPositionFEN, true); }
			else { loadFromFEN({}, true); }
		}
	}


	// Selection
	for (auto& piece : position.pieceList) {
		if (piece) {
			if (piece->contains(mousePos)) {
				mouseSelecting = true;
			}
		}
	}
}

void Board::updateS(sf::Vector2f& mousePos, bool isFocused)
{
	mouseSelecting = false;
	mouseClicked = false;
	selectedPos = { getLocalPosition(mousePos) };

	// Board Movement
	if (!scaleMode || (scaleMode && (!pieceMoving || (pieceMoving && !mouseMode))) && !calculatingPos && isFocused) {
		if (mouseMode) {
			if (!scaleMode) {
				setLocation(mousePos);
			}
			else {
				float distance = mousePos.x - initialMousePos.x + mousePos.y - initialMousePos.y;
				setScale(std::max(initialBoardScale + (distance / 500.0f), 0.01f));
			}
		}
	}
}

void Board::draw(sf::RenderWindow& window) const {
	window.draw(boardSprite);
	window.draw(lastMoveStart);
	window.draw(lastMoveDest);
	if (variant->name == "kingofthehill") {
		window.draw(kothShadow);
		window.draw(kothBackground);
	}
	else if (variant->name == "racingkings") {
		window.draw(rankBackground);
		window.draw(rankShadowTop);
	}
	if (selectedPiece) {
		window.draw(selectedPieceBackground);
	}
	if (!pieceMoving && selectedPiece && !isPromoting && !isGating && !calculatingPos && !stockfish.isCalculating && !mouseMode) {
		selectedPiece->drawSprites(window);
	}

	if (hasCheck) {
		for (auto& sprite : checkSprites) {
			window.draw(sprite);
		}
	}
	for (auto& obj : captureObjects) {
		window.draw(obj);
	}

	window.draw(ghostSprite);

	for (auto& piece : position.pieceList) {
		if (piece) {
			if (selectedPiece) {
				if (piece != selectedPiece) {
					piece->draw(window);
				}
			}
			else {
				piece->draw(window);
			}
		}
	}
	for (auto& piece : position.pieceList) {
		if (piece) {
			if (piece->hasTarget()) {
				piece->draw(window);
			}
		}
	}

	if (promotePiece || gatingPiece || (gatingKing && gatingRook)) {
		window.draw(promotionOverlay);
	}
	if (isPromoting && promotePiece && !calculatingPos && !stockfish.isCalculating) {
		for (auto& promotionPiece : promotePieces) {
			window.draw(promotionPiece.backgroundSprite);
			window.draw(promotionPiece.sprite);
		}
	}
	if (isGating && gatingPiece && !calculatingPos && !stockfish.isCalculating) {
		for (auto& gatePiece : gatingPieces) {
			window.draw(gatePiece.backgroundSprite);
			window.draw(gatePiece.sprite);
		}
	}
	if (isGating && gatingKing && gatingRook && !calculatingPos && !stockfish.isCalculating) {
		for (auto& gatePiece : gatingKingPieces) {
			window.draw(gatePiece.backgroundSprite);
			window.draw(gatePiece.sprite);
		}
		for (auto& gatePiece : gatingRookPieces) {
			window.draw(gatePiece.backgroundSprite);
			window.draw(gatePiece.sprite);
		}
	}
	if (selectedPiece) {
		selectedPiece->draw(window);
	}
	if (variant->nCheckCounting) {
		window.draw(whiteCheckCount);
		window.draw(blackCheckCount);
		window.draw(whiteCheckText);
		window.draw(blackCheckText);
	}
	window.draw(optionChangeOverlay);
	window.draw(optionChangeText);
	window.draw(gameEndOverlay);
	window.draw(gameEndText);
	window.draw(dropPieceBackgroundW);
	window.draw(dropPieceBackgroundB);
	for (auto& piece : whiteDropPieces) {
		piece.draw(window);
	}
	for (auto& piece : blackDropPieces) {
		piece.draw(window);
	}
	if (holdingDropPiece && !calculatingPos && !stockfish.isCalculating) {
		selectedDropPiece.draw(window);
	}

	if (timeEnabled) {
		window.draw(whiteTimeOutline);
		window.draw(blackTimeOutline);
		window.draw(whiteTimeBG);
		window.draw(blackTimeBG);
		window.draw(whiteTimeText);
		window.draw(blackTimeText);
	}
}

// Hand, Arrow, SizeAll, blockCursor
int Board::getMouseCursor(bool& blockCursor) const
{
	if (calculatingPos && !position.gameEnded && !AI_Only) {
		if (mouseClicked && Chess::isPieceAt(selectedPos, position.pieceList) && getPieceFromCurrentPosition(sf::Vector2i(selectedPos))->isWhite() == whiteToPlay) {
			if (stockfishEnabled) {
				if (whiteToPlay == playerSideWhite) {
					blockCursor = true;
				}
			}
			else {
				blockCursor = true;
			}
		}
	}
	else {
		blockCursor = false;
	}
	if (animationFinished) {
		if (mouseMode) {
			return 2;
		}
		else if (mouseSelecting) {
			if (blockCursor) {
				return 3;
			}
			else {
				return 0;
			}
		}
		else { return 1; }
	}
	else {
		return 3;
	}
}