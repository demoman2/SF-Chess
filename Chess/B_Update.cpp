#include "Board.h"

void Board::handleEvent(std::optional<sf::Event> event, sf::Vector2f& mousePos, sf::Image& boardSpriteSheet, std::vector<sf::Image>& pieceSpriteSheets, bool isFocused, bool controlClicked)
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
						startingWhiteTime += sf::seconds(5);
						setOptionChange("Initial White Time", startingWhiteTime.asSeconds());
					}
					else {
						whiteTime += sf::seconds(5);
						if (stockfish.isCalculating && !stockfish.getSettings().usesFixedTime) { stockfish.stopCalculating(); }
					}
				}
				else if (blackTimeBG.getGlobalBounds().contains(mousePos)) {
					if (controlClicked) {
						startingBlackTime += sf::seconds(5);
						setOptionChange("Initial Black Time", startingBlackTime.asSeconds());
					}
					else {
						blackTime += sf::seconds(5);
						if (stockfish.isCalculating && !stockfish.getSettings().usesFixedTime) { stockfish.stopCalculating(); }
					}
				}
			}
			if (stockfishEnabled && whiteToPlay != playerSideWhite) {
				return;
			}
			std::shared_ptr<Piece> p = getPieceFromCurrentPosition(sf::Vector2i(selectedPos));
			if (p && promotePiece == nullptr && !pieceMoving && animationFinished && !calculatingPos && !stockfish.isCalculating && !mouseMode && !generatingMoves) {
				// Side to Play == Color
				if (whiteToPlay == (p->isWhite())) {
					if (selectedPiece && selectedPiece->hasID('k')) {
						std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(selectedPiece);
						if (!std::any_of(king->getCaptureCastleSquares().begin(), king->getCaptureCastleSquares().end(), [selectedPos = selectedPos](Chess::Square& square) { return square.pos == selectedPos; })) {
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
						startingWhiteTime = std::max(startingWhiteTime - sf::seconds(5), sf::Time::Zero);
						setOptionChange("Initial White Time", startingWhiteTime.asSeconds());
					}
					else {
						whiteTime = std::max(whiteTime - sf::seconds(5), sf::Time::Zero);
						if (stockfish.isCalculating && !stockfish.getSettings().usesFixedTime) { stockfish.stopCalculating(); }
					}
				}
				else if (blackTimeBG.getGlobalBounds().contains(mousePos)) {
					if (controlClicked) {
						startingBlackTime = std::max(startingBlackTime - sf::seconds(5), sf::Time::Zero);
						setOptionChange("Initial Black Time", startingBlackTime.asSeconds());
					}
					else {
						blackTime = std::max(blackTime - sf::seconds(5), sf::Time::Zero);
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
								gameType = Chess::UltraBullet;
							}
							else if (time < 180) {
								instantMove = true;
								gameType = Chess::Bullet;
							}
							else if (time < 480) {
								moveSpeed = 1.5f;
								gameType = Chess::Blitz;
							}
							else if (time < 1500) {
								moveSpeed = 1.0f;
								gameType = Chess::Rapid;
							}
							else {
								moveSpeed = 0.8f;
								gameType = Chess::Classical;
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
				if (keyPressed->code == sf::Keyboard::Key::I) {
					if (!pieceMoving && animationFinished && !calculatingPos) {
						scale(0.8f);
					}
				}
				else if (keyPressed->code == sf::Keyboard::Key::O) {
					endgamePosition = !endgamePosition;
					if (optionMode) {
						setOptionChange("Endgame Position", endgamePosition);
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

			if (keyPressed->code == sf::Keyboard::Key::D) {
				if (!pieceMoving && animationFinished && !calculatingPos) {
					scale(1.2f);
				}
			}
			else if (keyPressed->code == sf::Keyboard::Key::R) {
				if (!(!scaleMode && mouseMode && (pieceMoving || !animationFinished))) {
					scaleMode = !scaleMode;
					if (optionMode) {
						setOptionChange("Scale Mode", scaleMode);
					}
				}
			}
			else if (keyPressed->code == sf::Keyboard::Key::M) {
				if (mouseMode || (!mouseMode && (!scaleMode || (scaleMode && (!pieceMoving || (pieceMoving && !mouseMode)) && (animationFinished || (!animationFinished && !mouseMode)))) && !calculatingPos))
				{
					initialMousePos = mousePos;
					initialBoardScale = boardScale;
					mouseMode = !mouseMode;
				}
			}
			else if (keyPressed->code == sf::Keyboard::Key::P) {
				++pieceSet %= pieceSpriteSheets.size();
				setPieceSheet(pieceSpriteSheets, pieceSet);
				if (optionMode) {
					setOptionChange("Piece Set", pieceSet + 1);
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
			else if (keyPressed->code == sf::Keyboard::Key::F) {
				if (!stockfish.isCalculating) {
					stockfish.printPosition();
					std::cout << "COUNT: " << stockfish.getLegalMoveCount(stockfish.getFEN());
				}
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
					int v = (int)variant + 1;
					setVariant(static_cast<Chess::Variant>(v % 9));
					if (optionMode) {
						setOptionChange("Variant", variant);
					}
				}
			}
			else if (keyPressed->code == sf::Keyboard::Key::L) {
				if (!AI_Only) {
					if (stockfishEnabled && animationFinished) {
						AI_Only = true;
					}
				}
				else {
					AI_Only = false;
				}
				if (optionMode) {
					setOptionChange("AI Only", AI_Only);
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
					chess960Enabled = !chess960Enabled;
					stockfish.setVariant(variant, chess960Enabled);
					if (optionMode) {
						setOptionChange("Chess960 Enabled", chess960Enabled);
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
					setVariant(static_cast<Chess::Variant>(std::rand() % 9));
					if (optionMode) {
						setOptionChange("Variant", variant);
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
						stockfish.setVariant(variant, chess960Enabled);
						startedStockfish = true;
					}
				}
				if (stockfishEnabled && selectedPiece && playerSideWhite == whiteToPlay) {
					if (holdingPiece) {
						selectedPiece->setPosition(getGlobalPosition(selectedPiece->getLocalPos()));
					}
					selectedPiece.reset();
				}
			}
		}
		else if (keyPressed->code == sf::Keyboard::Key::Left) {
			if (positionHistory.size() > 1 && !calculatingPos && (!pieceMoving || (pieceMoving && changingPosition)) && animationFinished && !generatingMoves) {
				positionChangeThreads.emplace_back(true);
			}
		}
		else if (keyPressed->code == sf::Keyboard::Key::Right) {
			if (!positionHistoryF.empty() && !calculatingPos && (!pieceMoving || (pieceMoving && changingPosition)) && animationFinished && !generatingMoves) {
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
	if (variant == Chess::ThreeCheck) {
		if (!holdingPiece) {
			for (auto& piece : pieceList) {
				if (piece->hasID('k') && piece->isWhite()) {
					whiteCheckCount.setPosition({ piece->getGlobalPos().x - (boardMultiplier / 2.91f), piece->getGlobalPos().y - (boardMultiplier / 2.91f) });
					whiteCheckText.setPosition(whiteCheckCount.getPosition());
					break;
				}
			}
			for (auto& piece : pieceList) {
				if (piece->hasID('k') && !piece->isWhite()) {
					blackCheckCount.setPosition({ piece->getGlobalPos().x - (boardMultiplier / 2.91f), piece->getGlobalPos().y - (boardMultiplier / 2.91f) });
					blackCheckText.setPosition(blackCheckCount.getPosition());
					break;
				}
			}
		}
		switch (whiteChecks) {
		case 0:
			whiteCheckCount.setColor(sf::Color(100, 173, 105));
			whiteCheckText.setString("3");
			break;
		case 1:
			whiteCheckCount.setColor(sf::Color(242, 119, 58));
			whiteCheckText.setString("2");
			break;
		case 2:
			whiteCheckCount.setColor(sf::Color(210, 0, 0));
			whiteCheckText.setString("1");
			break;
		case 3:
			whiteCheckCount.setColor(sf::Color(45, 45, 45));
			whiteCheckText.setString("0");
			break;
		}
		switch (blackChecks) {
		case 0:
			blackCheckCount.setColor(sf::Color(100, 173, 105));
			blackCheckText.setString("3");
			break;
		case 1:
			blackCheckCount.setColor(sf::Color(242, 119, 58));
			blackCheckText.setString("2");
			break;
		case 2:
			blackCheckCount.setColor(sf::Color(210, 0, 0));
			blackCheckText.setString("1");
			break;
		case 3:
			blackCheckCount.setColor(sf::Color(45, 45, 45));
			blackCheckText.setString("0");
			break;
		}
	}
	else if (variant == Chess::FiveCheck) {
		if (!holdingPiece) {
			for (auto& piece : pieceList) {
				if (piece->hasID('k') && piece->isWhite()) {
					whiteCheckCount.setPosition({ piece->getGlobalPos().x - (boardMultiplier / 2.91f), piece->getGlobalPos().y - (boardMultiplier / 2.91f) });
					whiteCheckText.setPosition({ whiteCheckCount.getPosition().x - (11.0f * sizeMultiplier), whiteCheckCount.getPosition().y - (21.5f * sizeMultiplier) });
					break;
				}
			}
			for (auto& piece : pieceList) {
				if (piece->hasID('k') && piece->isBlack()) {
					blackCheckCount.setPosition({ piece->getGlobalPos().x - (boardMultiplier / 2.91f), piece->getGlobalPos().y - (boardMultiplier / 2.91f) });
					blackCheckText.setPosition({ blackCheckCount.getPosition().x - (11.0f * sizeMultiplier), blackCheckCount.getPosition().y - (21.5f * sizeMultiplier) });
					break;
				}
			}
		}
		switch (whiteChecks) {
		case 0:
			whiteCheckCount.setColor(sf::Color(65, 168, 224));
			whiteCheckText.setString("5");
			break;
		case 1:
			whiteCheckCount.setColor(sf::Color(66, 175, 137));
			whiteCheckText.setString("4");
			break;
		case 2:
			whiteCheckCount.setColor(sf::Color(100, 173, 105));
			whiteCheckText.setString("3");
			break;
		case 3:
			whiteCheckCount.setColor(sf::Color(242, 119, 58));
			whiteCheckText.setString("2");
			break;
		case 4:
			whiteCheckCount.setColor(sf::Color(210, 0, 0));
			whiteCheckText.setString("1");
			break;
		case 5:
			whiteCheckCount.setColor(sf::Color(45, 45, 45));
			whiteCheckText.setString("0");
			break;
		}
		switch (blackChecks) {
		case 0:
			blackCheckCount.setColor(sf::Color(65, 168, 224));
			blackCheckText.setString("5");
			break;
		case 1:
			blackCheckCount.setColor(sf::Color(66, 175, 137));
			blackCheckText.setString("4");
			break;
		case 2:
			blackCheckCount.setColor(sf::Color(100, 173, 105));
			blackCheckText.setString("3");
			break;
		case 3:
			blackCheckCount.setColor(sf::Color(242, 119, 58));
			blackCheckText.setString("2");
			break;
		case 4:
			blackCheckCount.setColor(sf::Color(210, 0, 0));
			blackCheckText.setString("1");
			break;
		case 5:
			blackCheckCount.setColor(sf::Color(45, 45, 45));
			blackCheckText.setString("0");
			break;
		}
	}

	// Board Animation
	if (std::any_of(pieceList.begin(), pieceList.end(), [](std::shared_ptr<Piece>& piece) { return piece->hasAnimationTarget(); })) {
		animationFinished = false;
		if (animationClock.getElapsedTime().asMilliseconds() > 250) {
			for (auto& piece : pieceList) {
				if (piece) { piece->updatePosition(moveSpeed, deltaTime); }
			}
		}
	}
	else { animationFinished = true; }

	if (!gameEnded) {
		if (stockfish.movePlayed) {
			stockfish.movePlayed = false;
			if (stockfishEnabled && !stockfish.getMove().empty()) { playMove(stockfish.getMove(), instantMove); }
			stockfish.resetMove();
		}

		if (std::any_of(pieceList.begin(), pieceList.end(), [](std::shared_ptr<Piece>& piece) { return piece->hasTarget(); })) {
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
							std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(castleKing);
							calculatingPos = true;
							std::thread postMoveF(&Board::postMove, this, pieceVector{ king, castleRook });
							castleKing.reset();
							castleRook.reset();
							postMoveF.detach();
						}
					}
					else {
						for (auto& piece : pieceList) {
							if (piece && piece->hasTarget()) {
								if (piece->getGlobalPos() == piece->getTarget()) {
									if (shouldPostMove) {
										calculatingPos = true;
										std::thread postMoveF(&Board::postMove, this, pieceVector{ piece });
										postMoveF.detach();
									}
									else {
										piece->setTarget({});
										shouldPostMove = true;
										if (promotePiece) {
											isPromoting = true;
											setupPromoteSprites();
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
								allPositionsPlayed.push_back(savePosition());
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
						for (auto& piece : pieceList) {
							if (piece && piece->hasTarget()) {
								if (piece->getGlobalPos() == piece->getTarget()) {
									sf::Vector2f pieceTarget = piece->getTarget();
									piece->setTarget({});
									if (forwardMove) {
										if (atomicExplosionEffect && variant == Chess::Atomic && getPieceFromCurrentPosition(getLocalPosition(pieceTarget)) != nullptr) {
											sf::Vector2i capturePos = getLocalPosition(pieceTarget);
											for (int x = -1; x <= 1; x++) {
												for (int y = -1; y <= 1; y++) {
													sf::Vector2i newPos = { capturePos.x + x, capturePos.y + y };
													if (Chess::isValidSquare(newPos)) {
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
										allPositionsPlayed.push_back(savePosition());
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
					std::string pieceMove = selectedPiece->getMoveIntersecting(mousePos);
					if (!pieceMove.empty()) {
						playMove(pieceMove, instantMove);
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
					std::string pieceMove = selectedPiece->getMoveIntersecting(mousePos);
					if (!pieceMove.empty()) {
						playMove(pieceMove, true);
						setSpriteVisible(ghostSprite, false);
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
		if (!pieceMoving && selectedPiece && !isPromoting && !calculatingPos && !mouseMode && !gameEnded) {
			selectedPiece->updateSprites(*boardTextures, mousePos, mouseSelecting);
		}

		if (selectedPiece) {
			if (holdingPiece) {
				ghostSprite.setTexture(selectedPiece->getTexture());
				ghostSprite.setPosition(getGlobalPosition(selectedPiece->getLocalPos()));
				setSpriteVisible(ghostSprite, true, 75);
				selectedPiece->setPosition(sf::Vector2f(mousePos));
			}
			else {
				setSpriteVisible(ghostSprite, false);
			}
		}

		// Promotion
		if (promotionEnabled) {
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
							sf::Vector2i localPos = getLocalPosition(promotePiece->getGlobalPos());
							std::string moveString = Chess::convertPositiontoNotation(promotePiece->getLocalPos()) + Chess::convertPositiontoNotation(localPos) + promotionPiece.id;
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

		// Drop Pieces
		if (dropsEnabled) {
			if (!calculatingPos && !stockfish.isCalculating && !gameEnded) {
				std::vector<std::shared_ptr<Chess::DropPiece>>& currentDropPieces = whiteToPlay ? whiteDropPieces : blackDropPieces;
				Chess::PColor currentColor = whiteToPlay ? Chess::White : Chess::Black;
				for (auto& piece : currentDropPieces) {
					if (piece->mouseSelecting(mousePos) && !holdingDropPiece) {
						mouseSelecting = true;
						if (mouseClicked) {
							selectedDropPiece.set(getTextureFromID(piece->id, currentColor), piece->dropSquares, piece->id);
							holdingDropPiece = true;
						}
						piece->background.setColor({ 255, 255, 255, Interpolate(piece->background.getColor().a, 100, 0.3f * deltaTime * 60) });
					}
					else {
						piece->background.setColor({ 255, 255, 255, Interpolate(piece->background.getColor().a, 0, 0.3f * deltaTime * 60) });
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
			sf::Time& whiteT = whiteTime, blackT = blackTime;
			if (AI_Time) {
				if (playerSideWhite) {
					whiteT = playerTime;
					blackT = AITime;
				}
				else {
					whiteT = AITime;
					blackT = playerTime;
				}
			}
			if ((delayClock.isRunning() && delayClock.getElapsedTime().asMilliseconds() < 100) || isPaused) {
				timeClock.reset();
			}
			else if (!timeClock.isRunning() && allPositionsPlayed.size() > 2 && animationFinished && !pieceMoving && !isPaused) {
				timeClock.start();
			}
			if (timeClock.isRunning() && animationFinished && !pieceMoving && !isPaused && !generatingMoves) {
				if (whiteToPlay) {
					sf::Time temp = whiteT;
					whiteT -= timeClock.restart();
					if (temp.asSeconds() >= 10 && whiteT.asSeconds() < 10 && !AI_Only && (!stockfishEnabled || stockfishEnabled && playerSideWhite == whiteToPlay)) {
						lowTimeSound.play();
					}
					if (whiteT < sf::Time::Zero) { whiteT = sf::Time::Zero; }
				}
				else {
					sf::Time temp = blackT;
					blackT -= timeClock.restart();
					if (temp.asSeconds() >= 10 && blackT.asSeconds() < 10 && !AI_Only && (!stockfishEnabled || stockfishEnabled && playerSideWhite == whiteToPlay)) {
						lowTimeSound.play();
					}
					if (blackT < sf::Time::Zero) { blackT = sf::Time::Zero; }
				}
				if (whiteT == sf::Time::Zero) {
					gameEnded = true;
				}
				else if (blackT == sf::Time::Zero) {
					gameEnded = true;
				}
			}


			std::string minutes = std::to_string((int)std::fmodf(std::floor(whiteT.asSeconds() / 60.0f), 60.0f));
			std::string seconds =  std::to_string((int)std::fmodf(whiteT.asSeconds(), 60.0f));
			std::string hours = std::to_string((int)std::floor(whiteT.asSeconds() / 3600.0f));
			if (minutes.size() == 1) { minutes.insert(0, "0"); }
			if (seconds.size() == 1) { seconds.insert(0, "0"); }
			if (hours.size() == 1) { hours.insert(0, "0"); }

			if (whiteT.asSeconds() >= 3600) {
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
			if (millisecondsCondition(whiteT)) {
				int millis = whiteT.asMilliseconds() % 1000;
				millis = std::floor(millis / 100.0f);
				std::string milliseconds = std::to_string(millis);
				if (whiteT.asSeconds() >= 3600) {
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

			minutes = std::to_string((int)std::fmodf(std::floor(blackT.asSeconds() / 60.0f), 60.0f));
			seconds = std::to_string((int)std::fmodf(blackT.asSeconds(), 60.0f));
			hours = std::to_string((int)std::floor(blackT.asSeconds() / 3600.0f));
			if (minutes.size() == 1) { minutes.insert(0, "0"); }
			if (seconds.size() == 1) { seconds.insert(0, "0"); }
			if (hours.size() == 1) { hours.insert(0, "0"); }

			if (blackT.asSeconds() >= 3600) {
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
			if (millisecondsCondition(blackT)) {
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
				if (whiteT.asSeconds() < 10) {
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
				if (blackT.asSeconds() < 10) {
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
	}
	else {
		if (std::none_of(pieceList.begin(), pieceList.end(), [](std::shared_ptr<Piece>& piece) { return piece->hasTarget(); })) {
			pieceMoving = false;
		}
		if (animationFinished && !pieceMoving && !calculatingPos && !stockfish.isCalculating) {
			if (repeatFEN) { loadFromFEN(newPositionFEN, true); }
			else { loadFromFEN({}, true); }
			gameShouldEnd = false;
		}
	}

	// Selection
	for (auto& piece : pieceList) {
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

void Board::draw(sf::RenderWindow& window) {
	window.draw(boardSprite);
	window.draw(lastMoveStart);
	window.draw(lastMoveDest);
	if (variant == Chess::KOTH) {
		window.draw(kothShadow);
		window.draw(kothBackground);
	}
	else if (variant == Chess::RacingKings) {
		window.draw(rankBackground);
		window.draw(rankShadowTop);
	}
	if (selectedPiece) {
		window.draw(selectedPieceBackground);
	}
	if (!pieceMoving && selectedPiece && !isPromoting && !calculatingPos && !stockfish.isCalculating && !mouseMode && !gameEnded) {
		selectedPiece->drawSprites(window);
	}

	if (hasCheck) { window.draw(checkSprite); }
	for (auto& obj : captureObjects) {
		window.draw(obj);
	}

	window.draw(ghostSprite);

	for (auto& piece : pieceList) {
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
	for (auto& piece : pieceList) {
		if (piece) {
			if (piece->hasTarget()) {
				piece->draw(window);
			}
		}
	}

	if (promotePiece) {
		window.draw(promotionOverlay);
	}
	if (isPromoting && promotePiece && !calculatingPos && !stockfish.isCalculating) {
		for (auto& promotionPiece : promotePieces) {
			window.draw(promotionPiece.backgroundSprite);
			window.draw(promotionPiece.sprite);
		}
	}
	if (selectedPiece) {
		selectedPiece->draw(window);
	}
	if (variant == Chess::ThreeCheck || variant == Chess::FiveCheck) {
		window.draw(whiteCheckCount);
		window.draw(blackCheckCount);
		window.draw(whiteCheckText);
		window.draw(blackCheckText);
	}
	window.draw(optionChangeOverlay);
	window.draw(optionChangeText);
	if (dropsEnabled) {
		window.draw(dropPieceBackgroundW);
		window.draw(dropPieceBackgroundB);
		for (auto& piece : whiteDropPieces) {
			piece->draw(window);
		}
		for (auto& piece : blackDropPieces) {
			piece->draw(window);
		}
		if (holdingDropPiece && !calculatingPos && !stockfish.isCalculating && !gameEnded) {
			selectedDropPiece.draw(window);
		}
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

// Hand, Arrow, SizeAll, Hourglass
int Board::getMouseCursor(bool& hourGlass) const
{
	if (calculatingPos && !gameEnded && !AI_Only) {
		if (mouseClicked && Chess::isPieceAt(selectedPos, pieceList) && getPieceFromCurrentPosition(sf::Vector2i(selectedPos))->isWhite() == whiteToPlay) {
			if (stockfishEnabled) {
				if (whiteToPlay == playerSideWhite) {
					hourGlass = true;
				}
			}
			else {
				hourGlass = true;
			}
		}
	}
	else {
		hourGlass = false;
	}
	if (animationFinished) {
		if (mouseMode) {
			return 2;
		}
		else if (mouseSelecting) {
			if (hourGlass) {
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