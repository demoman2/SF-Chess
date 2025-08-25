#include "Board.h"

// FEN
void Board::loadFromFEN(std::optional<std::string> FEN_ID, bool init, bool first)
{
	if (startedStockfish) { stockfish.setVariant(variant, chess960Enabled); }
	if (stockfish.isCalculating) { stockfish.stopCalculating(); }
	setSpriteVisible(ghostSprite, false);
	moves = "";
	if (!first) {
		playerTime = startingPlayerTime;
		AITime = startingAITime;
		whiteTime = startingWhiteTime;
		blackTime = startingBlackTime;
	}
	whiteChecks = 0;
	blackChecks = 0;
	gameEnded = false;
	hasCheck = false;
	eighthRankWhite = false;
	dropsEnabled = false;
	castlingEnabled = true;
	promotionEnabled = true;
	checksEnabled = true;
	timeClock.reset();
	lastMoveStart.setPosition({ -1000, -1000 });
	lastMoveDest.setPosition({ -1000, -1000 });
	lastMoveStartLocal = { 100, 100 };
	lastMoveDestLocal = { 100, 100 };
	captureObjects.clear();
	positionHistory.clear();
	whiteDropPieces.clear();
	blackDropPieces.clear();
	selectedPiece.reset();
	capturePiece.reset();
	pieceList.clear();
	allPositionsPlayed.clear();
	whitePromotePieces.clear();
	blackPromotePieces.clear();
	captureSound.setVolume(15);
	captureSound.setBuffer(soundBuffers->at(1));
	captureThreshold = 1.0f;

	// Variant Setup
	whitePromotePieces.emplace_back('q', sf::Vector2f{ pieceScale * 0.82f, pieceScale * 0.82f });
	whitePromotePieces.emplace_back('n', sf::Vector2f{ pieceScale * 0.775f, pieceScale * 0.775f });
	whitePromotePieces.emplace_back('b', sf::Vector2f{ pieceScale * 0.8f, pieceScale * 0.8f });
	whitePromotePieces.emplace_back('r', sf::Vector2f{ pieceScale * 0.785f, pieceScale * 0.785f });

	blackPromotePieces.emplace_back('q', sf::Vector2f{ pieceScale * 0.82f, pieceScale * 0.82f });
	blackPromotePieces.emplace_back('n', sf::Vector2f{ pieceScale * 0.775f, pieceScale * 0.775f });
	blackPromotePieces.emplace_back('b', sf::Vector2f{ pieceScale * 0.8f, pieceScale * 0.8f });
	blackPromotePieces.emplace_back('r', sf::Vector2f{ pieceScale * 0.785f, pieceScale * 0.785f });

	if (variant == Chess::Antichess) {
		castlingEnabled = false;
		checksEnabled = false;
		whitePromotePieces.emplace_back('k', sf::Vector2f{ pieceScale * 0.785f, pieceScale * 0.785f });
		blackPromotePieces.emplace_back('k', sf::Vector2f{ pieceScale * 0.785f, pieceScale * 0.785f });
	}
	else if (variant == Chess::Atomic) {
		captureSound.setVolume(9);
		captureSound.setBuffer(soundBuffers->at(3));
		captureThreshold = 30.0f;
	}
	else if (variant == Chess::RacingKings) {
		castlingEnabled = false;
	}
	else if (variant == Chess::Crazyhouse) {
		dropsEnabled = true;
	}

	startingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w AHah - 0 1";
	if (FEN_ID.has_value()) {
		startingFEN = FEN_ID.value();
		if (variant == Chess::ThreeCheck) {
			if (startingFEN.find('+') == std::string::npos && findNthOf(startingFEN, ' ', 4) != 0) {
				startingFEN.insert(findNthOf(startingFEN, ' ', 4), " 3+3");
			}
		}
		else if (variant == Chess::FiveCheck) {
			if (startingFEN.find('+') == std::string::npos && findNthOf(startingFEN, ' ', 4) != 0) {
				startingFEN.insert(findNthOf(startingFEN, ' ', 4), " 5+5");
			}
		}
		if (dropsEnabled) {
			if (startingFEN.find('+') == std::string::npos && findNthOf(startingFEN, ' ', 1) != 0) {
				startingFEN.insert(findNthOf(startingFEN, ' ', 1), "[]");
			}
		}
	}
	else {
		if (endgamePosition) {
			std::string fen;
			if (variant == Chess::Antichess) {
				fen = Chess::getRandomLineFrom("assets/fen/antichess_endgame.txt");
				if (variant == Chess::ThreeCheck) {
					startingFEN = fen.substr(0, fen.size() - 3);
					startingFEN += "3+3 0 1";
				}
				else if (variant == Chess::FiveCheck) {
					startingFEN = fen.substr(0, fen.size() - 3);
					startingFEN += "5+5 0 1";
				}
				else if (variant == Chess::Crazyhouse) {
					startingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR[] w KQkq - 0 1";
				}
				else if (!fen.empty()) {
					startingFEN = fen;
				}
			}
			else if (variant == Chess::Standard) {
				fen = Chess::getRandomLineFrom("assets/fen/endgame.txt");
				if (variant == Chess::ThreeCheck) {
					startingFEN = fen.substr(0, fen.size() - 3);
					startingFEN += "3+3 0 1";
				}
				else if (variant == Chess::FiveCheck) {
					startingFEN = fen.substr(0, fen.size() - 3);
					startingFEN += "5+5 0 1";
				}
				else if (variant == Chess::Crazyhouse) {
					startingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR[] w KQkq - 0 1";
				}
				else {
					if (!fen.empty()) {
						startingFEN = fen;
					}
				}
			}
			else {
				startingFEN = getNewFEN();
			}
		}
		else {
			startingFEN = getNewFEN();
		}
	}

	if (dropsEnabled) {
		setupDropSprites();
	}

	if (timeEnabled && !overridePieceSpeed) {
		instantMove = false;
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
	}
	if (!startingFEN.empty() && startingFEN.at(startingFEN.find_first_of(' ') + 1) == 'b') {
		whiteToPlay = false;
	}
	else if (!startingFEN.empty() && startingFEN.at(startingFEN.find_first_of(' ') + 1) == 'w') {
		whiteToPlay = true;
	}

	std::optional<bool>& playerWhite = first ? white : newPositionWhite;

	if (!endgamePosition) {
		if (cycleSides && !first) {
			playerSideWhite = !playerSideWhite;
		}
		else {
			if (playerWhite.has_value()) {
				playerSideWhite = playerWhite.value();
			}
			else { playerSideWhite = randomBool(); }
		}
	}
	else { playerSideWhite = whiteToPlay; }

	if (stockfishEnabled && !AI_Only) {
		if (!playerSideWhite) {
			isFlipped = true;
		}
		else {
			isFlipped = false;
		}
	}
	else {
		if (!whiteToPlay) {
			isFlipped = true;
		}
		else {
			isFlipped = false;
		}
	}

	bool t = isAnimated;
	if (!init) { isAnimated = false; }
	pieceList = generatePositionFromFENID(startingFEN);
	if (FEN_ID.has_value()) { startingFEN = getCurrentFEN(); }
	if (!init) { isAnimated = t; }
	generateLegalMoves(true);
}

void Board::loadFromPosition(const Chess::Position& position)
{
	pieceList = Chess::copyPieceVec(position.pieceList, false, true);
	whiteChecks = position.whiteChecks;
	blackChecks = position.blackChecks;
	whiteToPlay = position.whiteToPlay;
	hasCheck = position.hasCheck;
	eighthRankWhite = position.eighthRankWhite;
	fullMoves = position.fullMoves;
	halfMoves = position.halfMoves;
	lastMoveStartLocal = position.lastMoveStartLocal;
	lastMoveDestLocal = position.lastMoveDestLocal;
	moves = position.moves;
	lastMoveStart.setPosition(getGlobalPosition(lastMoveStartLocal));
	lastMoveDest.setPosition(getGlobalPosition(lastMoveDestLocal));
	if (hasCheck) {
		checkSprite.setPosition(getGlobalPosition(position.checkPos));
	}
	if (timeEnabled) {
		whiteTime = position.whiteTime;
		blackTime = position.blackTime;
		timeClock.reset();
	}
	setPieceSprites(true);
	delayClock.restart();
}

// Sprite Setup
void Board::setupDropSprites() {
	selectedDropPiece.sprite.setOrigin(selectedDropPiece.sprite.getLocalBounds().getCenter());
	selectedDropPiece.sprite.setScale(sf::Vector2f(pieceScale, pieceScale));
	float boardX = boardTexture.getSize().x * boardScale;
	float boardY = boardTexture.getSize().y * boardScale;
	dropPieceCount.x = 5;
	if (dropPieceCount.x >= 8) {
		dropPieceSquareSize.x = (boardY - 15.0f) / (float)dropPieceCount.x;
	}
	else if (dropPieceCount.x < 4) {
		dropPieceSquareSize.x = ((boardY - 15.0f) / 8.0f) + (60.0f / (float)dropPieceCount.x);
	}
	else {
		dropPieceSquareSize.x = ((boardY - 15.0f) / 8.0f) + (85.0f / (float)dropPieceCount.x);
	}
	dropPieceWidth.x = dropPieceSquareSize.x;
	dropPieceHeight.x = dropPieceSquareSize.x * dropPieceCount.x;

	dropPieceBackgroundW.setColor(sf::Color(210, 210, 210));
	dropPieceBackgroundW.setScale({ dropPieceWidth.x / dropPieceBackgroundW.getTexture().getSize().x, dropPieceHeight.x / dropPieceBackgroundW.getTexture().getSize().y });
	dropPieceBackgroundW.setOrigin(dropPieceBackgroundW.getLocalBounds().getCenter());
	dropPieceBackgroundW.setPosition({ boardOffset.x - (dropPieceSquareSize.x / 1.25f), (boardY / 2.0f) + boardOffset.y });

	dropPieceCount.y = 5;
	if (dropPieceCount.y >= 8) {
		dropPieceSquareSize.y = (boardY - 15.0f) / (float)dropPieceCount.y;
	}
	else if (dropPieceCount.y < 4) {
		dropPieceSquareSize.y = ((boardY - 15.0f) / 8.0f) + (60.0f * 1.0f / (float)dropPieceCount.y);
	}
	else {
		dropPieceSquareSize.y = ((boardY - 15.0f) / 8.0f) + (85.0f * 1.0f / (float)dropPieceCount.y);
	}
	dropPieceWidth.y = dropPieceSquareSize.y;
	dropPieceHeight.y = dropPieceSquareSize.y * dropPieceCount.y;

	dropPieceBackgroundB.setColor(sf::Color(210, 210, 210));
	dropPieceBackgroundB.setScale({ dropPieceWidth.y / dropPieceBackgroundB.getTexture().getSize().x, dropPieceHeight.y / dropPieceBackgroundB.getTexture().getSize().y });
	dropPieceBackgroundB.setOrigin(dropPieceBackgroundB.getLocalBounds().getCenter());
	dropPieceBackgroundB.setPosition({ boardOffset.x + (dropPieceSquareSize.y / 1.25f + boardX), (boardY / 2.0f) + boardOffset.y });

	sf::Vector2f bgStart = { dropPieceBackgroundW.getPosition().y - (dropPieceBackgroundW.getTexture().getSize().y * dropPieceBackgroundW.getScale().y / 2.0f) + (dropPieceSquareSize.y / 2.0f), dropPieceBackgroundB.getPosition().y - (dropPieceBackgroundB.getTexture().getSize().y * dropPieceBackgroundB.getScale().y / 2.0f) + (dropPieceSquareSize.y / 2.0f) };

	std::vector<sf::Texture>& bT = *boardTextures;
	if (variant == Chess::Crazyhouse) {
		whiteDropPieces.emplace_back(std::make_shared<Chess::DropPawn>('p', getTextureFromID('p', Chess::White), bT.at(15), bT.at(14), *textFont, dropPieceBackgroundW.getPosition(), dropPieceSquareSize.x, bgStart.x, whiteDropPieces.size()));
		whiteDropPieces.emplace_back(std::make_shared<Chess::DropPiece>('n', getTextureFromID('n', Chess::White), bT.at(15), bT.at(14), *textFont, dropPieceBackgroundW.getPosition(), dropPieceSquareSize.x, bgStart.x, whiteDropPieces.size()));
		whiteDropPieces.emplace_back(std::make_shared<Chess::DropPiece>('b', getTextureFromID('b', Chess::White), bT.at(15), bT.at(14), *textFont, dropPieceBackgroundW.getPosition(), dropPieceSquareSize.x, bgStart.x, whiteDropPieces.size()));
		whiteDropPieces.emplace_back(std::make_shared<Chess::DropPiece>('r', getTextureFromID('r', Chess::White), bT.at(15), bT.at(14), *textFont, dropPieceBackgroundW.getPosition(), dropPieceSquareSize.x, bgStart.x, whiteDropPieces.size()));
		whiteDropPieces.emplace_back(std::make_shared<Chess::DropPiece>('q', getTextureFromID('q', Chess::White), bT.at(15), bT.at(14), *textFont, dropPieceBackgroundW.getPosition(), dropPieceSquareSize.x, bgStart.x, whiteDropPieces.size()));

		blackDropPieces.emplace_back(std::make_shared<Chess::DropPawn>('p', getTextureFromID('p', Chess::Black), bT.at(15), bT.at(14), *textFont, dropPieceBackgroundB.getPosition(), dropPieceSquareSize.y, bgStart.y, blackDropPieces.size()));
		blackDropPieces.emplace_back(std::make_shared<Chess::DropPiece>('n', getTextureFromID('n', Chess::Black), bT.at(15), bT.at(14), *textFont, dropPieceBackgroundB.getPosition(), dropPieceSquareSize.y, bgStart.y, blackDropPieces.size()));
		blackDropPieces.emplace_back(std::make_shared<Chess::DropPiece>('b', getTextureFromID('b', Chess::Black), bT.at(15), bT.at(14), *textFont, dropPieceBackgroundB.getPosition(), dropPieceSquareSize.y, bgStart.y, blackDropPieces.size()));
		blackDropPieces.emplace_back(std::make_shared<Chess::DropPiece>('r', getTextureFromID('r', Chess::Black), bT.at(15), bT.at(14), *textFont, dropPieceBackgroundB.getPosition(), dropPieceSquareSize.y, bgStart.y, blackDropPieces.size()));
		blackDropPieces.emplace_back(std::make_shared<Chess::DropPiece>('q', getTextureFromID('q', Chess::Black), bT.at(15), bT.at(14), *textFont, dropPieceBackgroundB.getPosition(), dropPieceSquareSize.y, bgStart.y, blackDropPieces.size()));
	}
}

void Board::setupPromoteSprites()
{
	promotePieces.clear();
	if (promotePiece) {
		int x = getLocalPosition(promotePiece->getGlobalPos()).x;
		if (promotePiece->isWhite()) {
			for (int y = 0; y < whitePromotePieces.size(); y++) {
				Chess::IPromotePiece& initialPiece = whitePromotePieces.at(y);
				promotePieces.emplace_back(initialPiece, getTextureFromID(initialPiece.id, Chess::White), boardTextures->at(6), sf::Vector2i{ x, Chess::reverseY(y + 1) }, boardOffset, boardSize, boardMultiplier, pieceScale, boardSprite.getScale(), isFlipped, promotionSquareColor);
			}
			promotePiece->setTarget({});
		}
		else {
			for (int y = 0; y < blackPromotePieces.size(); y++) {
				Chess::IPromotePiece& initialPiece = blackPromotePieces.at(y);
				promotePieces.emplace_back(initialPiece, getTextureFromID(initialPiece.id, Chess::Black), boardTextures->at(6), sf::Vector2i{ x, y + 1 }, boardOffset, boardSize, boardMultiplier, pieceScale, boardSprite.getScale(), isFlipped, promotionSquareColor);
			}
			promotePiece->setTarget({});
		}
	}
}

void Board::setPieceSprites(bool makePieceSprites) {
	if (makePieceSprites) {
		for (auto& piece : pieceList) {
			piece->makeSprite(getTextureFromID(piece->id, piece->color), pieceScale, boardOffset, boardSize, boardMultiplier, isFlipped);
		}
	}
	for (auto& piece : pieceList) { piece->setSprites(boardTextures->at(0), boardOffset, boardSize, boardMultiplier, pieceScale, isFlipped); }
	if (dropsEnabled) {
		std::vector<std::shared_ptr<Chess::DropPiece>>& currentDropPieces = whiteToPlay ? whiteDropPieces : blackDropPieces;
		for (auto& piece : currentDropPieces) {
			piece->setupSprites(boardTextures->at(0), boardOffset, boardSize, boardMultiplier, pieceScale, isFlipped);
		}
	}
}

// Board Movement, Textures
void Board::moveBy(float x, float y)
{
	offset.x += x;
	offset.y += y;
	boardOffset.x += x;
	boardOffset.y += y;
	for (auto& piece : pieceList) {
		piece->move(x, y);
	}
	boardSprite.move({ x, y });
	checkSprite.move({ x, y });
	selectedPieceBackground.move({ x, y });
	lastMoveStart.move({ x, y });
	lastMoveDest.move({ x, y });
	kothBackground.move({ x, y });
	kothShadow.move({ x, y });
	rankBackground.move({ x, y });
	rankShadowTop.move({ x, y });
	whiteCheckCount.move({ x, y });
	blackCheckCount.move({ x, y });
	dropPieceBackgroundW.move({ x, y });
	dropPieceBackgroundB.move({ x, y });
	promotionOverlay.move({ x, y });
	optionChangeOverlay.move({ x, y });
	optionChangeText.move({ x, y });
	ghostSprite.move({ x, y });
	whiteTimeBG.move({ x, y });
	whiteTimeText.move({ x, y });
	blackTimeBG.move({ x, y });
	blackTimeText.move({ x, y });
	whiteTimeOutline.move({ x, y });
	blackTimeOutline.move({ x, y });
	if (dropsEnabled) {
		for (auto& piece : whiteDropPieces) {
			piece->move(x, y);
		}
		for (auto& piece : blackDropPieces) {
			piece->move(x, y);
		}
	}
	setPieceSprites();
}

void Board::scale(float scale)
{
	boardScale *= scale;
	boardMultiplier *= scale;
	pieceScale *= scale;
	dropPieceSquareSize.x *= scale;
	dropPieceSquareSize.y *= scale;
	sizeMultiplier *= scale;
	boardSprite.setScale({ boardScale, boardScale });
	boardOffset.x = (windowSize.x / 2.0f) - ((boardTexture.getSize().x * boardScale) / 2.0f) + offset.x;
	boardOffset.y = (windowSize.y / 2.0f) - ((boardTexture.getSize().y * boardScale) / 2.0f) + offset.y;
	boardSize = { (boardTexture.getSize().x * boardScale), (boardTexture.getSize().y * boardScale) };
	for (auto& piece : pieceList) {
		piece->updateScale(pieceScale, boardOffset, boardSize, boardMultiplier, isFlipped);
	}
	checkSprite.setScale({ boardScale, boardScale });
	selectedPieceBackground.setScale({ boardScale, boardScale });
	lastMoveStart.setScale({ boardScale, boardScale });
	lastMoveDest.setScale({ boardScale, boardScale });
	kothBackground.setScale({ boardScale, boardScale });
	kothShadow.setScale({ boardScale, boardScale });
	rankBackground.setScale({ boardScale, boardScale });
	rankShadowTop.setScale({ boardScale, boardScale });
	whiteCheckCount.setScale({ boardScale / 3.0f, boardScale / 3.0f });
	blackCheckCount.setScale({ boardScale / 3.0f, boardScale / 3.0f });
	optionChangeText.setScale(boardSprite.getScale());
	optionChangeText.setPosition(getGlobalPosition(sf::Vector2f{ 4.5f, 4.5f }));
	optionChangeOverlay.setScale(boardSprite.getScale());
	optionChangeOverlay.setPosition(boardSprite.getPosition());
	dropPieceBackgroundW.setScale({ dropPieceBackgroundW.getScale().x * scale, dropPieceBackgroundW.getScale().y * scale });
	dropPieceBackgroundB.setScale({ dropPieceBackgroundB.getScale().x * scale, dropPieceBackgroundB.getScale().y * scale });
	dropPieceBackgroundW.setPosition({ boardOffset.x - (dropPieceSquareSize.x / 1.25f), dropPieceBackgroundW.getPosition().y });
	dropPieceBackgroundB.setPosition({ boardOffset.x + (dropPieceSquareSize.y / 1.25f + boardTexture.getSize().x * boardScale), dropPieceBackgroundW.getPosition().y });
	ghostSprite.setScale({ pieceScale, pieceScale });

	blackTimeBG.setScale({ boardScale * 1.02f, boardScale * 1.02f });
	blackTimeBG.setPosition({ getGlobalPosition(sf::Vector2f{6.5f, 8.5f}, false) });
	blackTimeBG.move({ 0, -(blackTimeBG.getTexture().getSize().y * blackTimeBG.getScale().y / 2.0f) - 1 });
	blackTimeOutline.setScale({ boardScale * 1.11f, boardScale * 1.129f });
	blackTimeOutline.setPosition({ getGlobalPosition(sf::Vector2f{6.5f, 8.5f}, false) });
	blackTimeOutline.move({ 0, -(blackTimeOutline.getTexture().getSize().y * blackTimeOutline.getScale().y / 2.0f) - 1 });
	blackTimeText.setPosition({ blackTimeBG.getPosition() });
	blackTimeText.setScale({ boardScale, boardScale });
	whiteTimeBG.setScale({ boardScale * 1.02f, boardScale * 1.02f });
	whiteTimeBG.setPosition({ getGlobalPosition(sf::Vector2f{2.5f, 8.5f}, false) });
	whiteTimeBG.move({ 0, -(whiteTimeBG.getTexture().getSize().y * whiteTimeBG.getScale().y / 2.0f) - 1 });
	whiteTimeOutline.setScale({ boardScale * 1.11f, boardScale * 1.129f });
	whiteTimeOutline.setPosition({ getGlobalPosition(sf::Vector2f{2.5f, 8.5f}, false) });
	whiteTimeOutline.move({ 0, -(whiteTimeOutline.getTexture().getSize().y * whiteTimeOutline.getScale().y / 2.0f) - 1 });
	whiteTimeText.setPosition({ whiteTimeBG.getPosition() });
	whiteTimeText.setScale({ boardScale, boardScale });

	if (hasCheck) {
		for (auto& piece : pieceList) {
			if (piece->hasID('k')) {
				std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(piece);
				if (king->inCheck) { checkSprite.setPosition(getGlobalPosition(king->getLocalPos())); }
			}
		}
	}

	if (variant == Chess::Crazyhouse) {
		sf::Vector2f bgStart = { dropPieceBackgroundW.getPosition().y - (dropPieceBackgroundW.getTexture().getSize().y * dropPieceBackgroundW.getScale().y / 2.0f) + (dropPieceSquareSize.y / 2.0f), dropPieceBackgroundB.getPosition().y - (dropPieceBackgroundB.getTexture().getSize().y * dropPieceBackgroundB.getScale().y / 2.0f) + (dropPieceSquareSize.y / 2.0f) };
		int i = 0;
		for (auto& piece : whiteDropPieces) {
			piece->scale(dropPieceSquareSize.x, dropPieceBackgroundW, bgStart.x, i);
			i++;
		}
		i = 0;
		for (auto& piece : blackDropPieces) {
			piece->scale(dropPieceSquareSize.y, dropPieceBackgroundB, bgStart.y, i);
			i++;
		}
	}
	setSpritePositions();
	setPieceSprites();
}

void Board::setScale(float newScale)
{
	scale(newScale / boardScale);
}

void Board::setLocation(sf::Vector2f pos)
{
	moveBy(pos.x - boardSprite.getPosition().x, pos.y - boardSprite.getPosition().y);
}

void Board::flipBoard()
{
	isFlipped = !isFlipped;
	for (auto& piece : pieceList) {
		piece->reversePosition(boardOffset, boardSize);
	}
	if (hasCheck) { checkSprite.setPosition(Chess::reversePosition(checkSprite.getPosition(), boardSize) + boardOffset + boardOffset); }
	setPieceSprites();
	setSpritePositions();
}

void Board::resetPosition()
{
	setLocation(startingOffset + sf::Vector2f{ windowSize.x / 2.0f, windowSize.y / 2.0f });
}

void Board::resetScale()
{
	scale(startingScale / boardScale);
}

void Board::setOptionChange(std::string string)
{
	optionChangeText.setString(string);
	optionChangeText.setOrigin(optionChangeText.getLocalBounds().getCenter());
	optionChangeText.setPosition(getGlobalPosition(sf::Vector2f{ 4.5f, 4.5f }));
	optionChangeText.setFillColor(sf::Color(255, 255, 255, 255));
	optionChangeOverlay.setFillColor(sf::Color(0, 0, 0, 170));
	optionClock.reset();
	optionClock.start();
}

void Board::setOptionChange(std::string string, bool v)
{
	std::ostringstream out;
	out << string << ": " << std::boolalpha << v;
	std::string n = out.str();
	size_t at = n.find(':') + 2;
	if (n.find(':') != std::string::npos) { n.at(at) = (char)std::toupper(n.at(at)); }
	optionChangeText.setString(n);
	optionChangeText.setOrigin({ optionChangeText.getLocalBounds().position.x + (optionChangeText.getLocalBounds().size.x / 2.0f), optionChangeText.getLocalBounds().position.y + (optionChangeText.getLocalBounds().size.y / 2.0f) });
	optionChangeText.setFillColor(sf::Color(255, 255, 255, 255));
	optionChangeOverlay.setFillColor(sf::Color(0, 0, 0, 170));
	optionClock.restart();
}

void Board::setOptionChange(std::string string, std::optional<bool> v)
{
	std::ostringstream out;
	if (v.has_value()) {
		out << string << ": " << std::boolalpha << v.value();
	}
	else {
		out << string << ": " << "N/A";
	}
	std::string n = out.str();
	size_t at = n.find(':') + 2;
	if (n.find(':') != std::string::npos) { n.at(at) = (char)std::toupper(n.at(at)); }
	optionChangeText.setString(n);
	optionChangeText.setOrigin({ optionChangeText.getLocalBounds().position.x + (optionChangeText.getLocalBounds().size.x / 2.0f), optionChangeText.getLocalBounds().position.y + (optionChangeText.getLocalBounds().size.y / 2.0f) });
	optionChangeText.setFillColor(sf::Color(255, 255, 255, 255));
	optionChangeOverlay.setFillColor(sf::Color(0, 0, 0, 170));
	optionClock.restart();
}

void Board::setOptionChange(std::string string, int v)
{
	optionChangeText.setString(string + ": " + std::to_string(v));
	optionChangeText.setOrigin({ optionChangeText.getLocalBounds().position.x + (optionChangeText.getLocalBounds().size.x / 2.0f), optionChangeText.getLocalBounds().position.y + (optionChangeText.getLocalBounds().size.y / 2.0f) });
	optionChangeText.setFillColor(sf::Color(255, 255, 255, 255));
	optionChangeOverlay.setFillColor(sf::Color(0, 0, 0, 170));
	optionClock.restart();
}

void Board::setOptionChange(std::string string, float v)
{
	std::ostringstream out;
	out << std::fixed << std::setprecision(2) << v;
	optionChangeText.setString(string + ": " + out.str());
	optionChangeText.setOrigin({ optionChangeText.getLocalBounds().position.x + (optionChangeText.getLocalBounds().size.x / 2.0f), optionChangeText.getLocalBounds().position.y + (optionChangeText.getLocalBounds().size.y / 2.0f) });
	optionChangeText.setFillColor(sf::Color(255, 255, 255, 255));
	optionChangeOverlay.setFillColor(sf::Color(0, 0, 0, 170));
	optionClock.restart();
}

void Board::setOptionChange(std::string string, Chess::Variant v)
{
	optionChangeText.setString(string + ": " + Chess::toString(v));
	optionChangeText.setOrigin(optionChangeText.getLocalBounds().getCenter());
	optionChangeText.setPosition(getGlobalPosition(sf::Vector2f{ 4.5f, 4.5f }));
	optionChangeText.setFillColor(sf::Color(255, 255, 255, 255));
	optionChangeOverlay.setFillColor(sf::Color(0, 0, 0, 170));
	optionClock.restart();
}

void Board::setSpritePositions()
{
	ghostSprite.setPosition({ -1000, -1000 });
	lastMoveStart.setPosition(getGlobalPosition(lastMoveStartLocal));
	lastMoveDest.setPosition(getGlobalPosition(lastMoveDestLocal));
	rankBackground.setPosition(getGlobalPosition(sf::Vector2f{ 4.5f, 8.0f }));
	rankShadowTop.setPosition(getGlobalPosition(sf::Vector2f{ 4.5f, 8.0f }));
	promotionOverlay.setPosition(boardSprite.getPosition());
	optionChangeOverlay.setPosition(boardSprite.getPosition());
	if (isFlipped) {
		rankShadowTop.setRotation(sf::degrees(180));
	}
	else {
		rankShadowTop.setRotation(sf::degrees(0));
	}
}

void Board::setPieceSheet(std::vector<sf::Image> sheets, int set)
{
	Chess::loadPieceSet(sheets.at(set), pieceTextures, pieceSize);
}

void Board::setBoardTexture(sf::Image& boardSheet, int set)
{
	boardTexture = Chess::loadBoard(boardSheet, set, 1024);
}

void Board::setPreviousPosition()
{
	castleKing.reset();
	castleRook.reset();
	selectedPiece.reset();
	setSpriteVisible(ghostSprite, false);
	bool hasGlobal = false, hasCastle = false;
	sf::Vector2f globalPos, globalRook, globalKing;
	std::shared_ptr<Piece> movePiece = nullptr, moveRook = nullptr;
	std::shared_ptr<King> moveKing = nullptr;
	if (changingPosition) {
		int count = 0;
		for (auto& piece : pieceList) {
			if (piece->hasTarget()) {
				count++;
			}
		}
		if (count == 2) {
			hasCastle = true;
		}
		if (!hasCastle) {
			for (auto& piece : pieceList) {
				if (piece->hasTarget()) {
					hasGlobal = true;
					globalPos = piece->getGlobalPos();
					movePiece = piece;
					piece->setPosition(piece->getTarget());
					piece->setTarget({});
					break;
				}
			}
		}
		else {
			for (auto& piece : pieceList) {
				if (piece->hasTarget()) {
					if (piece->hasID('k')) {
						hasGlobal = true;
						globalKing = piece->getGlobalPos();
						piece->setPosition(piece->getTarget());
						piece->setTarget({});
						movePiece = piece;
						moveKing = std::dynamic_pointer_cast<King>(piece);
					}
					else if (piece->hasID('r')) {
						hasGlobal = true;
						globalRook = piece->getGlobalPos();
						piece->setPosition(piece->getTarget());
						piece->setTarget({});
						moveRook = piece;
					}
				}
			}
		}
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
		changingPosition = false;
	}
	Chess::Position& newPosition = positionHistory.at(positionHistory.size() - 2);
	std::string newMove = positionHistory.back().move;
	positionHistoryF.insert(positionHistoryF.begin(), positionHistory.back());
	loadFromPosition(newPosition);
	if (std::isdigit(newMove.back()) && newMove.find('@') == std::string::npos) {
		changingPosition = true;
		int midPos = 0;
		for (int i = 1; i < newMove.size() - 1; i++) {
			if (!std::isdigit(newMove.at(i))) {
				midPos = i;
			}
		}
		sf::Vector2i piecePos = Chess::convertChessNotationtoPosition(newMove.substr(0, midPos));
		sf::Vector2i dest = Chess::convertChessNotationtoPosition(newMove.substr(midPos));
		std::shared_ptr<Piece> piece = getPieceFromCurrentPosition(piecePos);
		std::shared_ptr<Piece> capture = getPieceFromCurrentPosition(dest);
		std::shared_ptr<Piece> rook;

		if (piece) {
			bool closeCastle = false;
			if (capture) {
				if (capture->color == piece->color) {
					if (piece->hasID('k') && capture->hasID('r')) {
						closeCastle = true;
					}
					capture.reset();
				}
			}
			if (piece->hasID('k')) {
				std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(piece);
				int pieceX = piece->getLocalPos().x;
				if (!closeCastle) {
					if (dest.x > pieceX + 1) {
						rook = getPieceFromCurrentPosition({ king->Krook, piece->getLocalPos().y });
						if (rook) {
							castleKing = king;
							castleRook = rook;
							if (hasGlobal && hasCastle && moveRook == rook && moveKing == king) {
								castleRook->setPosition(globalRook);									
								castleKing->setPosition(globalKing);									
							}
							else {
								castleRook->setPosition(getGlobalPosition(sf::Vector2i{ 6, piece->getLocalPos().y }));
								castleKing->setPosition(getGlobalPosition(sf::Vector2i{ 7, piece->getLocalPos().y }));
							}
							castleRook->setTarget(getGlobalPosition(sf::Vector2i{ king->Krook, piece->getLocalPos().y }));
							castleKing->setTarget(getGlobalPosition(piecePos));
						}
					}
					else if (dest.x < pieceX - 1) {
						rook = getPieceFromCurrentPosition({ king->Qrook, piece->getLocalPos().y });
						if (rook) {
							castleKing = king;
							castleRook = rook;
							if (hasGlobal && hasCastle && moveRook == rook && moveKing == king) {
								castleRook->setPosition(globalRook);
								castleKing->setPosition(globalKing);
							}
							else {
								castleRook->setPosition(getGlobalPosition(sf::Vector2i{ 4, piece->getLocalPos().y }));
								castleKing->setPosition(getGlobalPosition(sf::Vector2i{ 3, piece->getLocalPos().y }));
							}
							castleRook->setTarget(getGlobalPosition(sf::Vector2i{ king->Qrook, piece->getLocalPos().y }));
							castleKing->setTarget(getGlobalPosition(piecePos));
						}
					}
				}
				else {
					if (dest.x > pieceX) {
						rook = getPieceFromCurrentPosition({ king->Krook, piece->getLocalPos().y });
						if (rook) {
							castleKing = king;
							castleRook = rook;
							if (hasGlobal && hasCastle && moveRook == rook && moveKing == king) {
								castleRook->setPosition(globalRook);
								castleKing->setPosition(globalKing);
							}
							else {
								castleRook->setPosition(getGlobalPosition(sf::Vector2i{ 6, piece->getLocalPos().y }));
								castleKing->setPosition(getGlobalPosition(sf::Vector2i{ 7, piece->getLocalPos().y }));
							}
							castleRook->setTarget(getGlobalPosition(sf::Vector2i{ king->Krook, piece->getLocalPos().y }));
							castleKing->setTarget(getGlobalPosition(piecePos));
						}
					}
					else if (dest.x < pieceX) {
						rook = getPieceFromCurrentPosition({ king->Qrook, piece->getLocalPos().y });
						if (rook) {
							castleKing = king;
							castleRook = rook;
							if (hasGlobal && hasCastle && moveRook == rook && moveKing == king) {
								castleRook->setPosition(globalRook);
								castleKing->setPosition(globalKing);
							}
							else {
								castleRook->setPosition(getGlobalPosition(sf::Vector2i{ 4, piece->getLocalPos().y }));
								castleKing->setPosition(getGlobalPosition(sf::Vector2i{ 3, piece->getLocalPos().y }));
							}
							castleRook->setTarget(getGlobalPosition(sf::Vector2i{ king->Qrook, piece->getLocalPos().y }));
							castleKing->setTarget(getGlobalPosition(piecePos));
						}
					}
				}
			}
			lastMoveStart.setPosition(getGlobalPosition(newPosition.lastMoveStartLocal));
			lastMoveDest.setPosition(getGlobalPosition(newPosition.lastMoveDestLocal));
			checkSprite.setPosition(getGlobalPosition(newPosition.checkPos));
			hasCheck = newPosition.hasCheck;
			if (!rook) { // No Castle
				if (hasGlobal && *movePiece == *piece) {
					piece->setPosition(globalPos);
				}
				else {
					piece->setPosition(getGlobalPosition(dest));
				}
				piece->setTarget(getGlobalPosition(piecePos));
			}
		}
		else {
			std::cout << "Target Piece For Position Change Was Not Found at Position: " << piecePos.x << ", " << piecePos.y << std::endl;
		}
	}
	if (!positionHistory.empty()) {
		positionHistory.pop_back();
	}
	if (!allPositionsPlayed.empty()) {
		allPositionsPlayed.pop_back();
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
}

void Board::setNextPosition()
{
	castleKing.reset();
	castleRook.reset();
	if (holdingPiece && selectedPiece) {
		selectedPiece->setPosition(getGlobalPosition(selectedPiece->getLocalPos()));
	}
	selectedPiece.reset();
	setSpriteVisible(ghostSprite, false);
	if (changingPosition && forwardMove && positionHistoryF.size() > 1) {
		for (auto& piece : pieceList) {
			if (piece->hasTarget()) {
				piece->setTarget({});
			}
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
		changingPosition = false;
	}
	if (!positionHistoryF.empty() && !forwardMove) {
		Chess::Position& newPosition = positionHistoryF.front();
		std::string newMove = newPosition.move;
		positionHistory.push_back(newPosition);
		if (std::isdigit(newMove.back()) && newMove.find('@') == std::string::npos) {
			forwardMove = true;
			changingPosition = true;
			int midPos = 0;
			for (int i = 1; i < newMove.size() - 1; i++) {
				if (!std::isdigit(newMove.at(i))) {
					midPos = i;
				}
			}
			sf::Vector2i piecePos = Chess::convertChessNotationtoPosition(newMove.substr(0, midPos));
			sf::Vector2i dest = Chess::convertChessNotationtoPosition(newMove.substr(midPos));
			std::shared_ptr<Piece> piece = getPieceFromCurrentPosition(piecePos);
			std::shared_ptr<Piece> capture = getPieceFromCurrentPosition(dest);
			if (piece) {
				bool closeCastle = false;
				if (capture) {
					if (capture->color == piece->color) {
						if (piece->hasID('k') && capture->hasID('r')) {
							closeCastle = true;
						}
						capture.reset();
					}
				}
				if (piece->hasID('k')) {
					std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(piece);
					int pieceX = piece->getLocalPos().x;
					if (!closeCastle) {
						if (dest.x > pieceX + 1) {
							std::shared_ptr<Piece> rook = getPieceFromCurrentPosition({ king->Krook, piece->getLocalPos().y });
							if (rook) {
								castleKing = piece;
								castleRook = rook;
								castleRook->setTarget(getGlobalPosition(sf::Vector2i{ 6, piece->getLocalPos().y }));
								castleKing->setTarget(getGlobalPosition(sf::Vector2i{ 7, piece->getLocalPos().y }));
							}
						}
						else if (dest.x < pieceX - 1) {
							std::shared_ptr<Piece> rook = getPieceFromCurrentPosition({ king->Qrook, piece->getLocalPos().y });
							if (rook) {
								castleKing = piece;
								castleRook = rook;
								castleRook->setTarget(getGlobalPosition(sf::Vector2i{ 4, piece->getLocalPos().y }));
								castleKing->setTarget(getGlobalPosition(sf::Vector2i{ 3, piece->getLocalPos().y }));
							}
						}
					}
					else {
						if (dest.x > pieceX) {
							std::shared_ptr<Piece> rook = getPieceFromCurrentPosition({ king->Krook, piece->getLocalPos().y });
							if (rook) {
								castleKing = piece;
								castleRook = rook;
								castleRook->setTarget(getGlobalPosition(sf::Vector2i{ 6, piece->getLocalPos().y }));
								castleKing->setTarget(getGlobalPosition(sf::Vector2i{ 7, piece->getLocalPos().y }));
							}
						}
						else if (dest.x < pieceX) {
							std::shared_ptr<Piece> rook = getPieceFromCurrentPosition({ king->Qrook, piece->getLocalPos().y });
							if (rook) {
								castleKing = piece;
								castleRook = rook;
								castleRook->setTarget(getGlobalPosition(sf::Vector2i{ 4, piece->getLocalPos().y }));
								castleKing->setTarget(getGlobalPosition(sf::Vector2i{ 3, piece->getLocalPos().y }));
							}
						}
					}
				}
				lastMoveStart.setPosition(getGlobalPosition(newPosition.lastMoveStartLocal));
				lastMoveDest.setPosition(getGlobalPosition(newPosition.lastMoveDestLocal));
				checkSprite.setPosition(getGlobalPosition(newPosition.checkPos));
				hasCheck = newPosition.hasCheck;
				std::shared_ptr<Piece> capture = getPieceFromCurrentPosition(dest);
				if (!closeCastle) {
					piece->setTarget(getGlobalPosition(dest));
					if (capture) {
						captureSound.play();
						ghostSprite.setTexture(capture->getTexture());
						ghostSprite.setPosition(capture->getGlobalPos());
						setSpriteVisible(ghostSprite, true, 75);
						capture->setVisible(false);
					}
					else {
						moveSound.play();
					}
				}
				else {
					moveSound.play();
				}
			}
			else {
				forwardMove = false;
				changingPosition = false;
				loadFromPosition(newPosition);
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
				std::cout << "Target Piece For Position Change Was Not Found at Position: " << piecePos.x << ", " << piecePos.y << std::endl;
			}
		}
		else {
			forwardMove = false;
			loadFromPosition(newPosition);
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
		}
	}
}

BoardSettings Board::getBoardArgs()
{
	float sizeM = sizeMultiplier;
	sf::Time whiteT = whiteTime, blackT = blackTime, sWhiteT = startingWhiteTime, sBlackT = startingBlackTime;
	if (AI_Time) {
		whiteT = AITime;
		blackT = playerTime;
		sWhiteT = startingAITime;
		sBlackT = startingPlayerTime;
	}
	if (timeEnabled) { sizeM *= ((float)boardTexture.getSize().y + whiteTimeOutline.getTexture().getSize().y * 1.129f) / (float)boardTexture.getSize().y; }
	return { variant, pieceSet, boardSet, AI_Time, stockfishEnabled, AI_Only, chess960Enabled, endgamePosition,
		repeatFEN, timeEnabled, offset.x, offset.y, sizeM, getCurrentFEN(), newPositionFEN, playerSideWhite, newPositionWhite,
		whiteT, blackT, timeIncrement, keyBindsEnabled, optionMode, autoFlip, atomicExplosionEffect, isAnimated,
		overridePieceSpeed, instantMove, isPaused, mouseMode, scaleMode, showMilliseconds, moveSpeed, promotionSquareSelectedColor,
		sWhiteT, sBlackT, millisecondsTime, millisecondsCondition,
		whiteUnit, blackUnit, incrUnit, startingWhiteUnit, startingBlackUnit, millisecondsConditionID, sharedTime, cycleSides };
}

const StockfishSettings& Board::getStockfishSettings()
{
	return stockfish.getSettings();
}

void Board::loadFromBoardArgs(const BoardSettings& bP, std::vector<sf::Image>& pieceSheets, sf::Image& boardSheet)
{
	if (stockfish.isCalculating && (!stockfish.getSettings().usesFixedTime &&
		((variant != bP.variant) || (whiteTime != bP.whiteTime) || (blackTime != bP.blackTime) || (timeIncrement != bP.timeIncrement)))) {
		stockfish.stopCalculating();
	}
	if (variant != bP.variant) {
		variant = bP.variant;
		if (variant == Chess::Antichess) {
			castlingEnabled = false;
			checksEnabled = false;
			whitePromotePieces.emplace_back('k', sf::Vector2f{ pieceScale * 0.785f, pieceScale * 0.785f });
			blackPromotePieces.emplace_back('k', sf::Vector2f{ pieceScale * 0.785f, pieceScale * 0.785f });
		}
		else if (variant == Chess::Atomic) {
			captureSound.setVolume(9);
			captureSound.setBuffer(soundBuffers->at(3));
			captureThreshold = 30.0f;
		}
		else if (variant == Chess::RacingKings) {
			castlingEnabled = false;
		}
		else if (variant == Chess::Crazyhouse) {
			dropsEnabled = true;
		}
		generateLegalMoves(false);
	}
	AI_Time = bP.AI_Time;
	whiteTime = bP.whiteTime;
	blackTime = bP.blackTime;
	if (AI_Time) {
		AITime = bP.whiteTime;
		playerTime = bP.blackTime;
		startingAITime = bP.startingWhiteTime;
		startingPlayerTime = bP.startingBlackTime;
	}
	pieceSet = bP.pieceSet;
	boardSet = bP.boardSet;
	stockfishEnabled = bP.AI;
	AI_Only = bP.AI_Only;
	AI_OnlyT = bP.AI_Only;
	if (chess960Enabled != bP.chess960) {
		chess960Enabled = bP.chess960;
		generateLegalMoves(false);
	}
	endgamePosition = bP.endgamePosition;
	isAnimated = bP.boardAnimated;
	repeatFEN = bP.repeatFEN;
	cycleSides = bP.cycleSides;
	moveBy(bP.xOffset - offset.x, bP.yOffset - offset.y);
	if (!pieceMoving) {
		setScale(bP.boardScale * startingScale);
		if (timeEnabled != bP.timeEnabled) {
			timeEnabled = bP.timeEnabled;
			if (!timeEnabled) {
				moveSpeedI = moveSpeed;
				instantMoveI = instantMove;
				moveSpeed = 1.0f;
				float scaleY = ((float)boardTexture.getSize().y + whiteTimeOutline.getTexture().getSize().y * 1.129f) / (float)boardTexture.getSize().y;
				startingScale *= scaleY;
				std::cout << scaleY << std::endl;
				std::cout << startingScale << std::endl;
				scale(scaleY);
				moveBy(0, -int(whiteTimeOutline.getTexture().getSize().y) * 1.129f / 2.0f);
				if (stockfish.isCalculating && !stockfish.getSettings().usesFixedTime) { stockfish.stopCalculating(); }
			}
			else {
				moveSpeed = moveSpeedI;
				instantMove = instantMoveI;
				float scaleY = (float)boardTexture.getSize().y / ((float)boardTexture.getSize().y + whiteTimeOutline.getTexture().getSize().y * 1.129f);
				startingScale *= scaleY;
				scale(scaleY);
				moveBy(0, int(whiteTimeOutline.getTexture().getSize().y) * 1.129f / 2.0f);
				if (stockfish.isCalculating && !stockfish.getSettings().usesFixedTime) { stockfish.stopCalculating(); }
			}
		}
	}
	keyBindsEnabled = bP.keybindsEnabled;
	optionMode = bP.showOptionChanges;
	autoFlip = bP.autoFlip;
	atomicExplosionEffect = bP.atomicExplosionEffect;
	isPaused = bP.isPaused;
	mouseMode = bP.followMouse;
	scaleMode = bP.scaleMouse;
	showMilliseconds = bP.showMilliseconds;
	moveSpeed = bP.pieceSpeed;
	promotionSquareSelectedColor = bP.promotionSquareColor;
	startingWhiteTime = bP.startingWhiteTime;
	startingBlackTime = bP.startingBlackTime;
	millisecondsTime = bP.millisecondsTime;
	millisecondsCondition = bP.millisecondsCondition;
	millisecondsConditionID = bP.millisecondsConditionID;
	sharedTime = bP.sharedTime;
	overridePieceSpeed = bP.overridePieceSpeed;
	if (!overridePieceSpeed) {
		instantMove = false;
		if (timeEnabled) {
			float time = std::min(startingWhiteTime.asSeconds(), startingBlackTime.asSeconds()) + (timeIncrement.asSeconds() * 40.0f);
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
		}
		else {
			moveSpeed = 1.0f;
		}
	}
	else {
		moveSpeed = bP.pieceSpeed;
	}
	instantMove = bP.instantPieces;
	whiteUnit = bP.whiteUnit;
	blackUnit = bP.blackUnit;
	incrUnit = bP.incrUnit;
	startingWhiteUnit = bP.startingWhiteUnit;
	startingBlackUnit = bP.startingBlackUnit;
	if (bP.white.has_value()) { playerSideWhite = bP.white.value(); }
	timeIncrement = bP.timeIncrement;
	newPositionWhite = bP.newPositionWhite;
	boardTexture = Chess::loadBoard(boardSheet, boardSet, 1024);
	white = bP.white;
	newPositionFEN = bP.newPositionFEN;
	Chess::loadPieceSet(pieceSheets.at(pieceSet), pieceTextures, pieceSize);
	if (bP.FEN.has_value() && bP.FEN != getCurrentFEN()) {
		loadFromFEN(bP.FEN.value(), false);
	}
	if (!startedStockfish) {
		stockfish.startStockfish();
		stockfish.setVariant(variant, chess960Enabled);
		startedStockfish = true;
	}
}

void Board::setStockfishSettings(const StockfishSettings& newSettings)
{
	const StockfishSettings& stockfishSettings = stockfish.getSettings();
	if (stockfish.isCalculating && (stockfishSettings.usesFixedTime != newSettings.usesFixedTime || (stockfishSettings.usesFixedTime && stockfishSettings.fixedTime != newSettings.fixedTime))) {
		stockfish.stopCalculating();
	}
	stockfish.loadSettings(newSettings);
}