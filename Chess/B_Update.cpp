#include "Board.h"

void Board::handleEvent(std::optional<sf::Event> event, sf::Image& boardSpriteSheet, std::vector<sf::Image>& pieceSpriteSheets, bool isFocused)
{
	if (!isFocused) { return; }
	if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
	{
		if (mouseButtonPressed->button == sf::Mouse::Button::Left)
		{
			mouseClicked = true;
			if (stockfishEnabled) {
				if (whiteToPlay != playerSideWhite && !isPaused) {
					return;
				}
			}
			std::shared_ptr<Piece> p = getPieceFromCurrentPosition(sf::Vector2i(selectedPos));
			if (p != nullptr && promotePiece == nullptr && !pieceMoving && animationFinished && !calculatingPos && !mouseMode && !generatingMoves) {
				// Side to Play == Color
				if (whiteToPlay == (p->isWhite())) {
					if (selectedPiece != nullptr && selectedPiece->IsA("King")) {
						std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(selectedPiece);
						if (!std::any_of(king->getCaptureCastleSquares()->begin(), king->getCaptureCastleSquares()->end(), [selectedPos = selectedPos](Chess::Square& square) { return square.pos == selectedPos; })) {
							selectedPiece = p;
						}
					}
					else {
						selectedPiece = p;
					}
				}
			}
		}
	}
	else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
	{
		if (keyPressed->code == sf::Keyboard::Key::N) {
			if (animationFinished && !pieceMoving && !calculatingPos && !generatingMoves) {
				if (repeatFEN) { setupFEN(FEN, true); }
				else { setupFEN({}, true); }
			}
		}
		else if (keyPressed->code == sf::Keyboard::Key::D) {
			if (!pieceMoving && animationFinished && !calculatingPos) {
				scale(1.2f);
			}
		}
		else if (keyPressed->code == sf::Keyboard::Key::I) {
			if (!pieceMoving && animationFinished && !calculatingPos) {
				scale(0.8f);
			}
		}
		else if (keyPressed->code == sf::Keyboard::Key::R) {
			scaleMode = !scaleMode;
			if (optionMode) {
				setOptionChange("Scale Mode", scaleMode);
			}
		}
		else if (keyPressed->code == sf::Keyboard::Key::T) {
			resetTransform();
		}
		else if (keyPressed->code == sf::Keyboard::Key::M) {
			if (!pieceMoving && animationFinished && !calculatingPos) {
				mouseMode = !mouseMode;
			}
		}
		else if (keyPressed->code == sf::Keyboard::Key::P) {
			++pieceSet %= pieceSpriteSheets.size();
			setPieceSheet(pieceSpriteSheets, pieceSet);
			if (optionMode) {
				setOptionChange("Piece Set", pieceSet);
			}
		}
		else if (keyPressed->code == sf::Keyboard::Key::B) {
			++boardSet %= 23;
			setBoardTexture(boardSpriteSheet, boardSet);
			if (optionMode) {
				setOptionChange("Board Set", boardSet);
			}
		}
		else if (keyPressed->code == sf::Keyboard::Key::F) {
			if (!pieceMoving && !calculatingPos) {
				stockfish.printPosition();
			}
			std::cout << "COUNT: " << stockfish.getLegalMoveCount(stockfish.getFEN());
		}
		else if (keyPressed->code == sf::Keyboard::Key::S) {
			if (isPaused) {
				if (!calculatingPos && !pieceMoving) {
					isPaused = false;
				}
			}
			else {
				isPaused = true;
			}
			if (optionMode) {
				setOptionChange("Paused", isPaused);
			}
			if (stockfishEnabled && !calculatingPos && !pieceMoving && selectedPiece != nullptr) {
				if (holdingPiece) {
					selectedPiece->setPosition(getGlobalPosition(selectedPiece->getLocalPos()));
				}
				selectedPiece.reset();
			}
		}
		else if (keyPressed->code == sf::Keyboard::Key::K) {
			if (!pieceMoving && !calculatingPos && animationFinished) {
				flipBoard();
			}
		}
		else if (keyPressed->code == sf::Keyboard::Key::V) {
			if (!pieceMoving && !calculatingPos && animationFinished && !generatingMoves) {
				int v = (int)variant + 1;
				setVariant(static_cast<Chess::Variant>(v % 9));
				if (optionMode) {
					setOptionChange("Variant", variant);
				}
			}
		}
		else if (keyPressed->code == sf::Keyboard::Key::A) {
			if (animationFinished) {
				stockfishEnabled = !stockfishEnabled;
				if (optionMode) {
					setOptionChange("AI Enabled", stockfishEnabled);
				}
				if (stockfishEnabled && !startedStockfish) {
					stockfish.startStockfish();
					stockfish.setVariant(variant, chess960Enabled);
					startedStockfish = true;
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
		else if (keyPressed->code == sf::Keyboard::Key::O) {
			endgamePosition = !endgamePosition;
			if (optionMode) {
				setOptionChange("Endgame Position", endgamePosition);
			}
		}
		else if (keyPressed->code == sf::Keyboard::Key::C) {
			if (!pieceMoving && !calculatingPos && animationFinished) {
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
		else if (keyPressed->code == sf::Keyboard::Key::H && !generatingMoves) {
			if (!pieceMoving && !calculatingPos && animationFinished) {
				setVariant(static_cast<Chess::Variant>(std::rand() % 9));
				if (optionMode) {
					setOptionChange("Variant", variant);
				}
			}
		}
		else if (keyPressed->code == sf::Keyboard::Key::G) {
			if (stockfishEnabled) {
				playerSideWhite = !playerSideWhite;
				setOptionChange("Player White", playerSideWhite);
			}
		}

	}
}

void Board::update(sf::Vector2f mousePos, bool isFocused)
{
	if (mouseClicked && mouseMode) { mouseMode = false; }

	if (animationFinished && optionClock.getElapsedTime().asMilliseconds() > 50) {
		optionChangeText.setFillColor({ 255, 255, 255, Interpolate(optionChangeText.getFillColor().a, 0, 0.15f) });
		optionChangeOverlay.setFillColor({ 0, 0, 0, Interpolate(optionChangeOverlay.getFillColor().a, 0, 0.15f) });
	}

	// Variant Extras
	if (variant == Chess::ThreeCheck) {
		if (!holdingPiece) {
			for (auto& piece : pieceList) {
				if (piece->IsA("King") && piece->isWhite()) {
					whiteCheckCount.setPosition({ piece->getGlobalPos().x - (boardMultiplier / 2.91f), piece->getGlobalPos().y - (boardMultiplier / 2.91f) });
					whiteCheckText.setPosition({ whiteCheckCount.getPosition().x - (11.0f * sizeMultiplier), whiteCheckCount.getPosition().y - (21.5f * sizeMultiplier) });
					break;
				}
			}
			for (auto& piece : pieceList) {
				if (piece->IsA("King") && !piece->isWhite()) {
					blackCheckCount.setPosition({ piece->getGlobalPos().x - (boardMultiplier / 2.91f), piece->getGlobalPos().y - (boardMultiplier / 2.91f) });
					blackCheckText.setPosition({ blackCheckCount.getPosition().x - (11.0f * sizeMultiplier), blackCheckCount.getPosition().y - (21.5f * sizeMultiplier) });
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
				if (piece->IsA("King") && piece->isWhite()) {
					whiteCheckCount.setPosition({ piece->getGlobalPos().x - (boardMultiplier / 2.91f), piece->getGlobalPos().y - (boardMultiplier / 2.91f) });
					whiteCheckText.setPosition({ whiteCheckCount.getPosition().x - (11.0f * sizeMultiplier), whiteCheckCount.getPosition().y - (21.5f * sizeMultiplier) });
					break;
				}
			}
			for (auto& piece : pieceList) {
				if (piece->IsA("King") && !piece->isWhite()) {
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
		for (auto& piece : pieceList) {
			piece->updatePosition();
		}
	}
	else { animationFinished = true; }

	if (!gameEnded) {
		if (stockfishEnabled && stockfish.movePlayed) {
			stockfish.movePlayed = false;
			calculatingPos = false;
			if (!stockfish.getMove().empty()) { playMove(stockfish.getMove(), false); }
			stockfish.resetMove();
		}

		if (std::any_of(pieceList.begin(), pieceList.end(), [](std::shared_ptr<Piece>& piece) { return piece->hasTarget(); })) {
			pieceMoving = true;
		}
		else { pieceMoving = false; }

		if (!calculatingPos) {
			// Stockfish Move
			if (stockfishEnabled && (AI_Only || (!AI_Only && whiteToPlay != playerSideWhite)) && animationFinished && !isPaused && !pieceMoving && !playingMove && !generatingMoves) {
				calculatingPos = true;
				std::thread moveT(&Stockfish::getBestMove, &stockfish, FEN, moves);
				moveT.detach();
			}

			// Movement
			if (castleKing != nullptr && castleRook != nullptr) {
				if (castleKing->getTarget() != castleKing->getGlobalPos() || castleRook->getTarget() != castleRook->getGlobalPos()) {
					castleKing->updatePosition();
					castleRook->updatePosition();
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
					if (piece != nullptr && piece->hasTarget()) {
						if (piece->getGlobalPos() == piece->getTarget()) {
							if (shouldPostMove) {
								calculatingPos = true;
								std::thread postMoveF(&Board::postMove, this, pieceVector{ piece });
								postMoveF.detach();
							}
							else {
								piece->setTarget({});
								shouldPostMove = true;
								if (promotePiece != nullptr) {
									isPromoting = true;
									setupPromoteSprites();
								}
							}
							break;
						}
						else {
							if (capturePiece != nullptr) {
								piece->updatePosition(captureThreshold);
								if (std::fmax(std::abs(piece->getGlobalPos().x - piece->getTarget().x), std::abs(piece->getGlobalPos().y - piece->getTarget().y)) < 5.0f) {
									setSpriteVisible(ghostSprite, false);
									capturePiece->setVisible(false);
								}
							}
							else {
								piece->updatePosition();
							}
						}
					}
				}
			}

			// Click Detection
			if (mouseClicked && !pieceMoving && animationFinished && !mouseMode && !holdingPiece) {
				if (selectedPiece != nullptr) {
					std::string pieceMove = selectedPiece->getMoveIntersecting(mousePos);
					if (!pieceMove.empty()) {
						playMove(pieceMove, false);
					}
					else if (selectedPiece->contains(mousePos)) { holdingPiece = true; }
				}
				if (getPieceFromCurrentPosition(sf::Vector2i(selectedPos)) == nullptr) {
					selectedPiece.reset();
				}
			}
			if (selectedPiece != nullptr) {
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
		if (!pieceMoving && selectedPiece != nullptr && !isPromoting && !calculatingPos && !mouseMode && !gameEnded) {
			selectedPiece->updateSprites(*boardTextures, mousePos, mouseSelecting);
		}

		if (selectedPiece != nullptr) {
			selectedPieceBackground.setPosition(getGlobalPosition(selectedPiece->getLocalPos()));
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
			if (isPromoting && promotePiece != nullptr && !calculatingPos) {
				for (auto& promotionPiece : promotePieces) {
					sf::Sprite& sprite = promotionPiece.sprite;
					sf::Sprite& backgroundSprite = promotionPiece.backgroundSprite;
					if (backgroundSprite.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
						mouseSelecting = true;
						backgroundSprite.setTexture(boardTextures->at(7));
						backgroundSprite.setColor(promotionSquareSelectedColor);
						sprite.setScale(Interpolate(sprite.getScale(), promotionPiece.initialScale + sf::Vector2f{ 0.035f, 0.035f }, 0.5f, 0.01f));
					}
					else {
						backgroundSprite.setTexture(boardTextures->at(6));
						backgroundSprite.setColor(promotionSquareColor);
						sprite.setScale(Interpolate(sprite.getScale(), promotionPiece.initialScale, 0.5f, 0.01f));
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
							playMove(moveString, false);
						}
					}
					if (!intersects) {
						shouldPostMove = false;
						isPromoting = false;
						promotePieces.clear();
						if (capturePiece != nullptr) {
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
			if (!calculatingPos && !gameEnded) {
				if (whiteToPlay) {
					for (auto& piece : whiteDropPieces) {
						if (piece.mouseSelecting(mousePos) && !holdingDropPiece) {
							mouseSelecting = true;
							if (mouseClicked) {
								selectedDropPiece.set(getTextureFromID(piece.id, Chess::White), piece.id);
								holdingDropPiece = true;
							}
							piece.background.setColor({ 255, 255, 255, Interpolate(piece.background.getColor().a, 100, 0.3f) });
						}
						else {
							piece.background.setColor({ 255, 255, 255, Interpolate(piece.background.getColor().a, 0, 0.3f) });
						}
					}
				}
				else {
					for (auto& piece : blackDropPieces) {
						if (piece.mouseSelecting(mousePos) && !holdingDropPiece) {
							mouseSelecting = true;
							if (mouseClicked) {
								selectedDropPiece.set(getTextureFromID(piece.id, Chess::Black), piece.id);
								holdingDropPiece = true;
							}
							piece.background.setColor({ 255, 255, 255, Interpolate(piece.background.getColor().a, 100, 0.3f) });
						}
						else {
							piece.background.setColor({ 255, 255, 255, Interpolate(piece.background.getColor().a, 0, 0.3f) });
						}
					}
				}

				if (holdingDropPiece) {
					if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
						holdingDropPiece = false;
						for (auto& square : dropSquares) {
							char id = selectedDropPiece.id;
							if (Chess::isValidDropSquare(id, square.pos)) {
								sf::Sprite& sprite = square.sprite.value();
								if (selectedDropPiece.sprite.getGlobalBounds().contains(sprite.getPosition())) {
									std::string moveString = "";
									moveString += (char)std::toupper(id);
									moveString += "@" + Chess::convertPositiontoNotation(square.pos);
									playMove(moveString, false);
								}
							}
						}
					}
					else {
						mouseSelecting = true;
						selectedDropPiece.sprite.setPosition(mousePos);
						for (auto& square : dropSquares) {
							if (Chess::isValidDropSquare(selectedDropPiece.id, square.pos)) {
								sf::Sprite sprite = square.sprite.value();
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

	}
	else {
		if (std::none_of(pieceList.begin(), pieceList.end(), [](std::shared_ptr<Piece>& piece) { return piece->hasTarget(); })) {
			pieceMoving = false;
		}
		if (animationFinished && !pieceMoving && !calculatingPos) {
			if (repeatFEN) { setupFEN(FEN, true); }
			else { setupFEN({}, true); }
			gameShouldEnd = false;
		}
	}

	// Selection
	for (auto& piece : pieceList) {
		if (piece != nullptr) {
			if (piece->contains(mousePos)) {
				mouseSelecting = true;
			}
		}
	}
}

void Board::updateS(sf::Vector2f mousePos, bool isFocused)
{
	mouseSelecting = false;
	mouseClicked = false;
	selectedPos = { getLocalPosition(mousePos) };

	// Board Movement
	if (!pieceMoving && animationFinished && !calculatingPos && isFocused) {
		if (!scaleMode) {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
				moveBy(3, 0);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
				moveBy(-3, 0);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
				moveBy(0, -3);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
				moveBy(0, 3);
			}
		}
		else {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
				scale(1.01f);
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
				scale(0.99f);
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
				scale(1.01f);
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
				scale(0.99f);
			}
		}
		if (mouseMode) {
			setLocation(mousePos);
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
	if (selectedPiece != nullptr) {
		window.draw(selectedPieceBackground);
	}
	if (!pieceMoving && selectedPiece != nullptr && !isPromoting && !calculatingPos && !mouseMode && !gameEnded) {
		selectedPiece->drawSprites(window);
	}

	if (hasCheck) { window.draw(checkSprite); }
	for (auto& obj : captureObjects) {
		window.draw(obj);
	}

	window.draw(ghostSprite);
	for (auto& piece : pieceList) {
		if (piece != nullptr) {
			if (selectedPiece != nullptr) {
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
		if (piece != nullptr) {
			if (piece->hasTarget()) {
				piece->draw(window);
			}
		}
	}
	if (promotePiece != nullptr) {
		window.draw(promotionOverlay);
	}
	if (isPromoting && promotePiece != nullptr && !calculatingPos) {
		for (auto& promotionPiece : promotePieces) {
			window.draw(promotionPiece.backgroundSprite);
			window.draw(promotionPiece.sprite);
		}
	}
	if (selectedPiece != nullptr) {
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
			piece.draw(window);
		}
		for (auto& piece : blackDropPieces) {
			piece.draw(window);
		}
		if (holdingDropPiece && !calculatingPos && !gameEnded) {
			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
				for (auto& square : dropSquares) {
					char id = selectedDropPiece.id;
					if (id != 'p' || (id == 'p' && square.pos.y != 1 && square.pos.y != 8)) {
						sf::Sprite sprite = square.sprite.value();
						window.draw(sprite);
					}
				}
				window.draw(selectedDropPiece.sprite);
			}
		}
	}
}

// Hand, Arrow, SizeAll, Hourglass
int Board::getMouseCursor(bool& hourGlass) const
{
	if (calculatingPos && !gameEnded && (!AI_Only || (AI_Only && isPaused))) {
		if (mouseClicked && Chess::isPieceAt(selectedPos, pieceList) && getPieceFromCurrentPosition(sf::Vector2i(selectedPos))->isWhite() == whiteToPlay) {
			if (stockfishEnabled) {
				if ((whiteToPlay == playerSideWhite) || isPaused) {
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
