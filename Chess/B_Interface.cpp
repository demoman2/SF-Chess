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

// FEN
void Board::loadFromFEN(const std::optional<std::string>& FEN_ID, bool init, bool first)
{
	stockfish.setVariant(variant->name, position.hasChess960);
	if (stockfish.isCalculating) { stockfish.stopCalculating(); }
	setSpriteVisible(ghostSprite, false);
	moves = "";
	if (!first) {
		playerTime = startingPlayerTime;
		AITime = startingAITime;
		whiteTime = startingWhiteTime;
		blackTime = startingBlackTime;
	}
	whiteChecksLeft = 0;
	blackChecksLeft = 0;
	position.gameEnded = false;
	hasCheck = false;
	position.extraFlagMove = false;
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
	position.pieceList.clear();
	allPositionsPlayed.clear();
	position.enPassantTarget = {};
	captureSound.setVolume(15);
	captureSound.setBuffer(soundBuffers->at(1));
	captureThreshold = 1.0f;

	// Variant Setup
	if (variant->atomicExplosions) {
		captureSound.setVolume(9);
		captureSound.setBuffer(soundBuffers->at(3));
		captureThreshold = 30.0f;
	}

	startingFEN = variant->startingFEN;
	if (FEN_ID.has_value()) {
		startingFEN = FEN_ID.value();
	}
	else {
		if (endgamePosition) {
			if (endgameVariants->count(variant->name) > 0) {
				std::string path = "assets/fen/endgame/";
				path += variant->name + ".txt";
				std::string fen = Chess::getRandomLineFrom(path);
				if (!fen.empty()) {
					startingFEN = fen;
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
	setupDropSprites();

	if (timeEnabled && !overridePieceSpeed) {
		instantMove = false;
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
	position.pieceList = generatePositionFromFENID(startingFEN);
	if (FEN_ID.has_value()) { startingFEN = getCurrentFEN(); }
	if (!init) { isAnimated = t; }
	generateLegalMoves(true);
}

void Board::loadFromPosition(const Chess::SavePosition& loadPosition)
{
	position.pieceList = Chess::copyPieceVec(loadPosition.pieceList, false, true, true);
	whiteChecksLeft = loadPosition.whiteChecksLeft;
	blackChecksLeft = loadPosition.blackChecksLeft;
	whiteToPlay = loadPosition.whiteToPlay;
	hasCheck = loadPosition.hasCheck;
	position.extraFlagMove = loadPosition.extraFlagMove;
	fullMoves = loadPosition.fullMoves;
	halfMoves = loadPosition.halfMoves;
	lastMoveStartLocal = loadPosition.lastMoveStartLocal;
	lastMoveDestLocal = loadPosition.lastMoveDestLocal;
	moves = loadPosition.moves;
	position.enPassantTarget = loadPosition.enPassantTarget;
	position.enPassantTripleTarget = loadPosition.enPassantTripleTarget;
	position.castlePieces = loadPosition.castlePieces;
	position.castlingRights = loadPosition.castlingRights;
	position.gameEnded = loadPosition.gameEnded;
	lastMoveStart.setPosition(getGlobalPosition(lastMoveStartLocal));
	lastMoveDest.setPosition(getGlobalPosition(lastMoveDestLocal));
	checkSprites.clear();
	if (hasCheck) {
		for (auto& pos : loadPosition.checkPositions) {
			sf::Sprite checkSprite(boardTextures->at(2));
			checkSprite.setOrigin(checkSprite.getLocalBounds().getCenter());
			checkSprite.setScale(boardSprite.getScale());
			checkSprite.setPosition(getGlobalPosition(pos));
			checkSprites.push_back(std::move(checkSprite));
		}
	}
	if (timeEnabled) {
		whiteTime = loadPosition.whiteTime;
		blackTime = loadPosition.blackTime;
		timeClock.reset();
	}
	for (auto& p : whiteDropPieces) { p.count = 0; }
	for (auto& p : blackDropPieces) { p.count = 0; }
	std::vector<std::string> splitString = split(loadPosition.FEN, ' ');
	std::vector<std::string> ranks = split(splitString.front(), '/');
	std::string back = ranks.back();
	size_t dropBegin = back.find('['), dropEnd = back.find(']');
	if (dropBegin != std::string::npos && dropEnd != std::string::npos) {
		std::string drop = ranks.back().substr(dropBegin + 1, dropEnd);
		drop.pop_back();
		for (auto& l : drop) {
			std::vector<Chess::DropPiece>& currentDropPieces = std::isupper(l) ? whiteDropPieces : blackDropPieces;
			for (auto& piece : currentDropPieces) {
				if (piece.id == std::tolower(l)) {
					piece.count += 1;
					break;
				}
			}
		}
	}
	for (auto& piece : whiteDropPieces) {
		piece.update();
	}
	for (auto& piece : blackDropPieces) {
		piece.update();
	}
	setPieceSprites(true);
	if (position.gameEnded && determineEnd() != Chess::Endgame::None) {
		std::string sideToPlayString = whiteToPlay ? "White " : "Black ";
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
	delayClock.restart();

}

// Sprite Setup
void Board::setupDropSprites() {
	std::set<char> whiteDrop, blackDrop;
	std::size_t start = startingFEN.find('['), end = startingFEN.find(']');
	whiteDrop.insert(variant->whiteDropPieces.begin(), variant->whiteDropPieces.end());
	blackDrop.insert(variant->blackDropPieces.begin(), variant->blackDropPieces.end());
	if (start != startingFEN.npos && end != startingFEN.npos) {
		for (auto p : startingFEN.substr(start + 1)) {
			if (!std::isalpha(p)) {
				break;
			}
			if (std::isupper(p)) {
				whiteDrop.insert(std::tolower(p));
			}
			else {
				blackDrop.insert(p);
			}
		}
	}

	dropPieceCount.x = whiteDrop.size();
	dropPieceCount.y = blackDrop.size();

	selectedDropPiece.sprite.setOrigin(selectedDropPiece.sprite.getLocalBounds().getCenter());
	selectedDropPiece.sprite.setScale(sf::Vector2f(pieceScale, pieceScale));
	float boardX = boardTexture.getSize().x * boardScale;
	float boardY = boardTexture.getSize().y * boardScale;
	if (dropPieceCount.x != 0) {
		setSpriteVisible(dropPieceBackgroundW, true);
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
	}
	else {
		setSpriteVisible(dropPieceBackgroundW, false);
	}
	if (dropPieceCount.y != 0) {
		setSpriteVisible(dropPieceBackgroundB, true);
		if (dropPieceCount.y >= 8) {
			dropPieceSquareSize.y = (boardY - 15.0f) / (float)dropPieceCount.y;
		}
		else if (dropPieceCount.y < 4) {
			dropPieceSquareSize.y = ((boardY - 15.0f) / 8.0f) + (60.0f / (float)dropPieceCount.y);
		}
		else {
			dropPieceSquareSize.y = ((boardY - 15.0f) / 8.0f) + (85.0f / (float)dropPieceCount.y);
		}
		dropPieceWidth.y = dropPieceSquareSize.y;
		dropPieceHeight.y = dropPieceSquareSize.y * dropPieceCount.y;

		dropPieceBackgroundB.setColor(sf::Color(210, 210, 210));
		dropPieceBackgroundB.setScale({ dropPieceWidth.y / dropPieceBackgroundB.getTexture().getSize().x, dropPieceHeight.y / dropPieceBackgroundB.getTexture().getSize().y });
		dropPieceBackgroundB.setOrigin(dropPieceBackgroundB.getLocalBounds().getCenter());
		dropPieceBackgroundB.setPosition({ boardOffset.x + (dropPieceSquareSize.y / 1.25f + boardX), (boardY / 2.0f) + boardOffset.y });
	}
	else {
		setSpriteVisible(dropPieceBackgroundB, false);
	}

	sf::Vector2f bgStart;
	bgStart.x = dropPieceBackgroundW.getPosition().y - (dropPieceBackgroundW.getTexture().getSize().y * dropPieceBackgroundW.getScale().y / 2.0f) + (dropPieceSquareSize.x / 2.0f),
	bgStart.y = dropPieceBackgroundB.getPosition().y - (dropPieceBackgroundB.getTexture().getSize().y * dropPieceBackgroundB.getScale().y / 2.0f) + (dropPieceSquareSize.y / 2.0f);

	const std::vector<sf::Texture>& bT = *boardTextures;
	std::for_each(variant->whiteDropPieces.begin(), variant->whiteDropPieces.end(), [&](char pieceID) {
		whiteDropPieces.emplace_back(pieceID, getTextureFromID(pieceID, Chess::PColor::White), bT.at(15), bT.at(14), *textFont, dropPieceBackgroundW.getPosition(), dropPieceSquareSize.x, bgStart.x, whiteDropPieces.size());
	});
	std::for_each(variant->blackDropPieces.begin(), variant->blackDropPieces.end(), [&](char pieceID) {
		blackDropPieces.emplace_back(pieceID, getTextureFromID(pieceID, Chess::PColor::Black), bT.at(15), bT.at(14), *textFont, dropPieceBackgroundB.getPosition(), dropPieceSquareSize.y, bgStart.y, blackDropPieces.size());
	});

	// add pieces that are in pocket if they dont exist
	if (start != startingFEN.npos && end != startingFEN.npos) {
		for (auto p : startingFEN.substr(start + 1)) {
			if (p == ']') {
				break;
			}
			if (std::isupper(p)) {
				p = std::tolower(p);
				if (std::none_of(whiteDropPieces.cbegin(), whiteDropPieces.cend(),
					[p](const Chess::DropPiece& pPiece) { return pPiece.id == p; })) {
					whiteDropPieces.emplace_back(p, getTextureFromID(p, Chess::PColor::White), bT.at(15), bT.at(14), *textFont, dropPieceBackgroundW.getPosition(), dropPieceSquareSize.x, bgStart.x, whiteDropPieces.size());
				}
			}
			else {
				if (std::none_of(blackDropPieces.cbegin(), blackDropPieces.cend(),
					[p](const Chess::DropPiece& pPiece) { return pPiece.id == p; })) {
					blackDropPieces.emplace_back(p, getTextureFromID(p, Chess::PColor::Black), bT.at(15), bT.at(14), *textFont, dropPieceBackgroundB.getPosition(), dropPieceSquareSize.y, bgStart.y, blackDropPieces.size());
				}
			}
		}
	}
}

void Board::setupPromoteSprites()
{
	promotePieces.clear();
	if (promotePiece) {
		sf::Vector2i pos = getLocalPosition(promotePiece->getGlobalPos());
		Chess::PieceType* validTypes = nullptr;
		for (auto& sq : promotePiece->getMoveSquares()) {
			if (sq.promoteSquare && sq.pos == pos) {
				validTypes = &sq.validPromotionTypes;
				break;
			}
		}
		if (!validTypes) {
			for (auto& sq : promotePiece->getCaptureSquares()) {
				if (sq.promoteSquare && sq.pos == pos) {
					validTypes = &sq.validPromotionTypes;
					break;
				}
			}
		}
		if (validTypes) {
			int y = 0;
			for (auto& type : *validTypes) {
				promotePieces.emplace_back(type, promotePieceSize, promotePiece->color, type == promotePiece->id ? boardTextures->at(17) : getTextureFromID(type, promotePiece->color), boardTextures->at(6),
					sf::Vector2i{ pos.x, promotePiece->isWhite() ? Chess::reverseY(y + 1, variant->boardSize) : y + 1 }, boardOffset, boardSize, boardMultiplier, pieceScale, boardSprite.getScale(),
					variant->boardSize, isFlipped, promotionSquareColor, false);
				y++;
			}
		}
		promotePiece->setTarget({});
	}
}

void Board::setupGatingSprites()
{
	gatingPieces.clear();
	gatingKingPieces.clear();
	gatingRookPieces.clear();
	if (gatingKing && gatingRook) {
		sf::Vector2i pos = gatingKing->getLocalPos();
		auto& currentDropPieces = whiteToPlay ? whiteDropPieces : blackDropPieces;
		int y = 0;
		for (auto& piece : currentDropPieces) {
			if (piece.count > 0 && !variant->pawnPieces.has(piece.id)) {
				gatingKingPieces.emplace_back(piece.id, promotePieceSize, gatingKing->color, getTextureFromID(piece.id, gatingKing->color), boardTextures->at(6),
					sf::Vector2i{ pos.x, gatingKing->isBlack() ? Chess::reverseY(y + 1, variant->boardSize) : y + 1 }, boardOffset, boardSize, boardMultiplier, pieceScale, boardSprite.getScale(),
					variant->boardSize, isFlipped, promotionSquareColor, false);
				y++;
			}
		}
		gatingKingPieces.emplace_back( gatingKing->id, promotePieceSize, gatingKing->color, boardTextures->at(17), boardTextures->at(6),
			sf::Vector2i{ pos.x, gatingKing->isBlack() ? Chess::reverseY(y + 1, variant->boardSize) : y + 1 }, boardOffset, boardSize, boardMultiplier, pieceScale, boardSprite.getScale(),
			variant->boardSize, isFlipped, promotionSquareColor, true);
		gatingKing->setTarget({});

		pos = gatingRook->getLocalPos();
		y = 0;
		for (auto& piece : currentDropPieces) {
			if (piece.count > 0 && !variant->pawnPieces.has(piece.id)) {
				gatingRookPieces.emplace_back(piece.id, promotePieceSize, gatingRook->color, getTextureFromID(piece.id, gatingRook->color), boardTextures->at(6),
					sf::Vector2i{ pos.x, gatingRook->isBlack() ? Chess::reverseY(y + 1, variant->boardSize) : y + 1 }, boardOffset, boardSize, boardMultiplier, pieceScale, boardSprite.getScale(),
					variant->boardSize, isFlipped, promotionSquareColor, false);
				y++;
			}
		}
		gatingRookPieces.emplace_back( gatingRook->id, promotePieceSize, gatingRook->color, boardTextures->at(17), boardTextures->at(6),
			sf::Vector2i{ pos.x, gatingRook->isBlack() ? Chess::reverseY(y + 1, variant->boardSize) : y + 1 }, boardOffset, boardSize, boardMultiplier, pieceScale, boardSprite.getScale(),
			variant->boardSize, isFlipped, promotionSquareColor, true);
		gatingRook->setTarget({});
	}
	else if (gatingPiece) {
		sf::Vector2i pos = gatingPiece->getLocalPos();
		auto& currentDropPieces = whiteToPlay ? whiteDropPieces : blackDropPieces;
		int y = 0;
		for (auto& piece : currentDropPieces) {
			if (piece.count > 0 && !variant->pawnPieces.has(piece.id)) {
				gatingPieces.emplace_back(piece.id, promotePieceSize, gatingPiece->color, getTextureFromID(piece.id, gatingPiece->color), boardTextures->at(6),
					sf::Vector2i{ pos.x, gatingPiece->isBlack() ? Chess::reverseY(y + 1, variant->boardSize) : y + 1 }, boardOffset, boardSize, boardMultiplier, pieceScale, boardSprite.getScale(),
					variant->boardSize, isFlipped, promotionSquareColor, false);
				y++;
			}
		}
		gatingPieces.emplace_back( gatingPiece->id, promotePieceSize, gatingPiece->color, boardTextures->at(17), boardTextures->at(6),
			sf::Vector2i{ pos.x, gatingPiece->isBlack() ? Chess::reverseY(y + 1, variant->boardSize) : y + 1 }, boardOffset, boardSize, boardMultiplier, pieceScale, boardSprite.getScale(),
			variant->boardSize, isFlipped, promotionSquareColor, true);
		gatingPiece->setTarget({});
	}
}

void Board::setPieceSprites(bool makePieceSprites) {
	if (makePieceSprites) {
		for (auto& piece : position.pieceList) {
			piece->makeSprite(getTextureFromID(piece->id, piece->color), pieceScale, boardOffset, boardSize, variant->boardSize, boardMultiplier, isFlipped);
		}
	}
	for (auto& piece : position.pieceList) { piece->setSprites(boardTextures->at(0), boardOffset, boardSize, variant->boardSize, boardMultiplier, pieceScale, isFlipped); }
	std::vector<Chess::DropPiece>& currentDropPieces = whiteToPlay ? whiteDropPieces : blackDropPieces;
	for (auto& piece : currentDropPieces) {
		piece.setupSprites(boardTextures->at(0), boardOffset, boardSize, variant->boardSize, boardMultiplier, isFlipped);
	}
}

// Board Movement, Textures
void Board::moveBy(float x, float y)
{
	offset.x += x;
	offset.y += y;
	boardOffset.x += x;
	boardOffset.y += y;
	for (auto& piece : position.pieceList) {
		piece->move(x, y);
	}
	boardSprite.move({ x, y });
	for (auto& s : checkSprites) {
		s.move({ x, y });
	}
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
	gameEndText.move({ x, y });
	gameEndOverlay.move({ x, y });
	ghostSprite.move({ x, y });
	whiteTimeBG.move({ x, y });
	whiteTimeText.move({ x, y });
	blackTimeBG.move({ x, y });
	blackTimeText.move({ x, y });
	whiteTimeOutline.move({ x, y });
	blackTimeOutline.move({ x, y });
	for (auto& piece : whiteDropPieces) {
		piece.move(x, y);
	}
	for (auto& piece : blackDropPieces) {
		piece.move(x, y);
	}
	setPieceSprites();
}

void Board::scale(float scale)
{
	std::vector<sf::Vector2i> checkPositions;
	for (auto& s : checkSprites) { checkPositions.push_back(getLocalPosition(s.getPosition())); }
	boardScale *= scale;
	boardMultiplier *= scale;
	pieceScale *= scale;
	dropPieceSquareSize.x *= scale;
	dropPieceSquareSize.y *= scale;
	promotePieceSize.x *= scale;
	promotePieceSize.y *= scale;
	sizeMultiplier *= scale;
	boardSprite.setScale({ boardScale, boardScale });
	boardOffset.x = (windowSize.x / 2.0f) - ((boardTexture.getSize().x * boardScale) / 2.0f) + offset.x;
	boardOffset.y = (windowSize.y / 2.0f) - ((boardTexture.getSize().y * boardScale) / 2.0f) + offset.y;
	boardSize = { (boardTexture.getSize().x * boardScale), (boardTexture.getSize().y * boardScale) };
	for (auto& piece : position.pieceList) {
		piece->updateScale(pieceScale, boardOffset, boardSize, variant->boardSize, boardMultiplier, isFlipped);
	}
	for (auto& s : checkSprites) {
		s.setScale({ boardScale, boardScale });
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
	optionChangeText.setScale(boardSprite.getScale());
	optionChangeText.setOrigin({ optionChangeText.getLocalBounds().position.x + (optionChangeText.getLocalBounds().size.x / 2.0f), optionChangeText.getLocalBounds().position.y + (optionChangeText.getLocalBounds().size.y / 2.0f) });
	whiteCheckText.setScale({ boardScale, boardScale });
	blackCheckText.setScale({ boardScale, boardScale });
	optionChangeText.setPosition(getGlobalPosition(sf::Vector2f{ variant->boardSize.x / 2.0f + 0.5f, variant->boardSize.y / 2.0f + 0.5f }));
	optionChangeOverlay.setScale(boardSprite.getScale());
	optionChangeOverlay.setPosition(boardSprite.getPosition());
	promotionOverlay.setScale(boardSprite.getScale());
	promotionOverlay.setPosition(boardSprite.getPosition());
	gameEndText.setScale(boardSprite.getScale());
	gameEndText.setOrigin({ gameEndText.getLocalBounds().position.x + (gameEndText.getLocalBounds().size.x / 2.0f), gameEndText.getLocalBounds().position.y + (gameEndText.getLocalBounds().size.y / 2.0f) });
	gameEndText.setPosition(getGlobalPosition(sf::Vector2f{ variant->boardSize.x / 2.0f + 0.5f, variant->boardSize.y / 2.0f + 0.5f }));
	gameEndOverlay.setScale(boardSprite.getScale());
	gameEndOverlay.setPosition(boardSprite.getPosition());
	dropPieceBackgroundW.setScale({ dropPieceBackgroundW.getScale().x * scale, dropPieceBackgroundW.getScale().y * scale });
	dropPieceBackgroundB.setScale({ dropPieceBackgroundB.getScale().x * scale, dropPieceBackgroundB.getScale().y * scale });
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

	if (hasCheck) {
		for (size_t i = 0; i < checkSprites.size(); i++) {
			auto& sprite = checkSprites.at(i);
			sprite.setPosition(getGlobalPosition(checkPositions.at(i)));
		}
	}

	sf::Vector2f bgStart = { dropPieceBackgroundW.getPosition().y - (dropPieceBackgroundW.getTexture().getSize().y * dropPieceBackgroundW.getScale().y / 2.0f) + (dropPieceSquareSize.y / 2.0f), dropPieceBackgroundB.getPosition().y - (dropPieceBackgroundB.getTexture().getSize().y * dropPieceBackgroundB.getScale().y / 2.0f) + (dropPieceSquareSize.y / 2.0f) };
	int i = 0;
	for (auto& piece : whiteDropPieces) {
		piece.scale(dropPieceSquareSize.x, dropPieceBackgroundW, bgStart.x, i);
		i++;
	}
	i = 0;
	for (auto& piece : blackDropPieces) {
		piece.scale(dropPieceSquareSize.y, dropPieceBackgroundB, bgStart.y, i);
		i++;
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
	for (auto& piece : position.pieceList) {
		piece->reversePosition(boardOffset, boardSize);
	}
	if (hasCheck) {
		for (auto& s : checkSprites) {
			s.setPosition(Chess::reversePosition(s.getPosition(), boardSize) + boardOffset + boardOffset);
		}
	}
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

void Board::setOptionChange(const std::string& string)
{
	optionChangeText.setString(string);
	optionChangeText.setOrigin(optionChangeText.getLocalBounds().getCenter());
	optionChangeText.setPosition(getGlobalPosition(sf::Vector2f{ variant->boardSize.x / 2.0f + 0.5f, variant->boardSize.y / 2.0f + 0.5f }));
	optionChangeText.setFillColor(sf::Color(255, 255, 255, 255));
	optionChangeOverlay.setFillColor(sf::Color(0, 0, 0, 170));
	optionClock.reset();
	optionClock.start();
}

void Board::setOptionChange(const std::string& string, bool v)
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

void Board::setOptionChange(const std::string& string, std::optional<bool> v)
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

void Board::setOptionChange(const std::string& string, int v)
{
	optionChangeText.setString(string + ": " + std::to_string(v));
	optionChangeText.setOrigin({ optionChangeText.getLocalBounds().position.x + (optionChangeText.getLocalBounds().size.x / 2.0f), optionChangeText.getLocalBounds().position.y + (optionChangeText.getLocalBounds().size.y / 2.0f) });
	optionChangeText.setFillColor(sf::Color(255, 255, 255, 255));
	optionChangeOverlay.setFillColor(sf::Color(0, 0, 0, 170));
	optionClock.restart();
}

void Board::setOptionChange(const std::string& string, float v)
{
	std::ostringstream out;
	out << std::fixed << std::setprecision(2) << v;
	optionChangeText.setString(string + ": " + out.str());
	optionChangeText.setOrigin({ optionChangeText.getLocalBounds().position.x + (optionChangeText.getLocalBounds().size.x / 2.0f), optionChangeText.getLocalBounds().position.y + (optionChangeText.getLocalBounds().size.y / 2.0f) });
	optionChangeText.setFillColor(sf::Color(255, 255, 255, 255));
	optionChangeOverlay.setFillColor(sf::Color(0, 0, 0, 170));
	optionClock.restart();
}

void Board::setOptionChange(const std::string& string, const Chess::Variant& v)
{
	optionChangeText.setString(string + ": " + v.name);
	optionChangeText.setOrigin({ optionChangeText.getLocalBounds().position.x + (optionChangeText.getLocalBounds().size.x / 2.0f), optionChangeText.getLocalBounds().position.y + (optionChangeText.getLocalBounds().size.y / 2.0f) });
	optionChangeText.setFillColor(sf::Color(255, 255, 255, 255));
	optionChangeOverlay.setFillColor(sf::Color(0, 0, 0, 170));
	optionClock.restart();
}

void Board::setGameEndText(const std::string& string)
{
	gameEndText.setString(string);
	gameEndText.setOrigin({ gameEndText.getLocalBounds().position.x + (gameEndText.getLocalBounds().size.x / 2.0f), gameEndText.getLocalBounds().position.y + (gameEndText.getLocalBounds().size.y / 2.0f) });
	gameEndText.setFillColor(sf::Color(255, 255, 255, 255));
	gameEndOverlay.setFillColor(sf::Color(0, 0, 0, 170));
	gameEndClock.restart();
}

void Board::setSpritePositions()
{
	ghostSprite.setPosition({ -1000, -1000 });
	lastMoveStart.setPosition(getGlobalPosition(lastMoveStartLocal));
	lastMoveDest.setPosition(getGlobalPosition(lastMoveDestLocal));
	rankBackground.setPosition(getGlobalPosition(sf::Vector2f{ variant->boardSize.x / 2.0f + 0.5f, static_cast<float>(variant->boardSize.y) }));
	rankShadowTop.setPosition(getGlobalPosition(sf::Vector2f{ variant->boardSize.x / 2.0f + 0.5f, static_cast<float>(variant->boardSize.y) }));
	promotionOverlay.setPosition(boardSprite.getPosition());
	optionChangeOverlay.setPosition(boardSprite.getPosition());
	promotionOverlay.setPosition(boardSprite.getPosition());
	gameEndOverlay.setPosition(boardSprite.getPosition());
	if (isFlipped) {
		rankShadowTop.setRotation(sf::degrees(180));
	}
	else {
		rankShadowTop.setRotation(sf::degrees(0));
	}
}

void Board::setBoardTexture(const sf::Image& boardSheet, int set)
{
	boardTexture = Chess::loadBoard(boardSheet, set, variant->boardSize);
}

void Board::setPieceTextures()
{
	pieceScale = boardScale * (128.0f / static_cast<float>(currentPieceTextures->begin()->second.getSize().x));
	for (auto& piece : position.pieceList) {
		piece->setTexture(getTextureFromID(piece->id, piece->color), pieceScale);
	}
	for (auto& piece : whiteDropPieces) {
		piece.setTexture(getTextureFromID(piece.id, Chess::PColor::White));
	}
	for (auto& piece : blackDropPieces) {
		piece.setTexture(getTextureFromID(piece.id, Chess::PColor::Black));
	}
	selectedDropPiece.setTexture(getTextureFromID(selectedDropPiece.id, whiteToPlay ? Chess::PColor::White : Chess::PColor::Black));
	for (auto& piece : promotePieces) {
		piece.setTexture(getTextureFromID(piece.id, Chess::PColor::Black));
	}
	for (auto& piece : gatingPieces) {
		piece.setTexture(getTextureFromID(piece.id, Chess::PColor::Black));
	}
	for (auto& piece : gatingKingPieces) {
		piece.setTexture(getTextureFromID(piece.id, Chess::PColor::Black));
	}
	for (auto& piece : gatingRookPieces) {
		piece.setTexture(getTextureFromID(piece.id, Chess::PColor::Black));
	}
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
	std::shared_ptr<Piece> moveKing = nullptr;
	// std::shared_ptr<King> moveKing = nullptr;
	if (changingPosition) {
		int count = 0;
		for (auto& piece : position.pieceList) {
			if (piece->hasTarget()) {
				count++;
			}
		}
		if (count == 2) {
			hasCastle = true;
		}
		if (!hasCastle) {
			for (auto& piece : position.pieceList) {
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
			for (auto& piece : position.pieceList) {
				if (piece->hasTarget()) {
					if (variant->castlingKingPiece.has(piece->id)) {
						hasGlobal = true;
						globalKing = piece->getGlobalPos();
						piece->setPosition(piece->getTarget());
						piece->setTarget({});
						movePiece = piece;
						moveKing = piece;
					}
					else if (variant->castlingRookPieces.has(piece->id)) {
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
		changingPosition = false;
	}
	Chess::SavePosition& newPosition = positionHistory.at(positionHistory.size() - 2);
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
					if (variant->castlingKingPiece.has(piece->id) && variant->castlingRookPieces.has(capture->id)) {
						closeCastle = true;
					}
					capture.reset();
				}
			}
			if (variant->castlingKingPiece.has(piece->id) && !instantMove) {
				int pieceX = piece->getLocalPos().x, offset = piece->isWhite() ? 0 : 2;
				if (!closeCastle) {
					if (dest.x > pieceX + 1) {
						rook = getPieceFromCurrentPosition({ position.castlePieces[0 + offset], piece->getLocalPos().y});
						if (rook) {
							castleKing = piece;
							castleRook = rook;
							if (hasGlobal && hasCastle && moveRook == rook && moveKing == piece) {
								castleRook->setPosition(globalRook);									
								castleKing->setPosition(globalKing);									
							}
							else {
								castleRook->setPosition(getGlobalPosition(sf::Vector2i{ variant->castleKDestination - 1, piece->getLocalPos().y }));
								castleKing->setPosition(getGlobalPosition(sf::Vector2i{ variant->castleKDestination, piece->getLocalPos().y }));
							}
							castleRook->setTarget(getGlobalPosition(sf::Vector2i{ position.castlePieces[0 + offset], piece->getLocalPos().y }));
							castleKing->setTarget(getGlobalPosition(piecePos));
						}
					}
					else if (dest.x < pieceX - 1) {
						rook = getPieceFromCurrentPosition({ position.castlePieces[1 + offset], piece->getLocalPos().y });
						if (rook) {
							castleKing = piece;
							castleRook = rook;
							if (hasGlobal && hasCastle && moveRook == rook && moveKing == piece) {
								castleRook->setPosition(globalRook);
								castleKing->setPosition(globalKing);
							}
							else {
								castleRook->setPosition(getGlobalPosition(sf::Vector2i{ variant->castleQDestination + 1, piece->getLocalPos().y }));
								castleKing->setPosition(getGlobalPosition(sf::Vector2i{ variant->castleQDestination, piece->getLocalPos().y }));
							}
							castleRook->setTarget(getGlobalPosition(sf::Vector2i{ position.castlePieces[1 + offset], piece->getLocalPos().y }));
							castleKing->setTarget(getGlobalPosition(piecePos));
						}
					}
				}
				else {
					if (dest.x > pieceX) {
						rook = getPieceFromCurrentPosition({ position.castlePieces[0 + offset], piece->getLocalPos().y });
						if (rook) {
							castleKing = piece;
							castleRook = rook;
							if (hasGlobal && hasCastle && moveRook == rook && moveKing == piece) {
								castleRook->setPosition(globalRook);
								castleKing->setPosition(globalKing);
							}
							else {
								castleRook->setPosition(getGlobalPosition(sf::Vector2i{ variant->castleKDestination - 1, piece->getLocalPos().y }));
								castleKing->setPosition(getGlobalPosition(sf::Vector2i{ variant->castleKDestination, piece->getLocalPos().y }));
							}
							castleRook->setTarget(getGlobalPosition(sf::Vector2i{ position.castlePieces[0 + offset], piece->getLocalPos().y }));
							castleKing->setTarget(getGlobalPosition(piecePos));
						}
					}
					else if (dest.x < pieceX) {
						rook = getPieceFromCurrentPosition({ position.castlePieces[1 + offset], piece->getLocalPos().y });
						if (rook) {
							castleKing = piece;
							castleRook = rook;
							if (hasGlobal && hasCastle && moveRook == rook && moveKing == piece) {
								castleRook->setPosition(globalRook);
								castleKing->setPosition(globalKing);
							}
							else {
								castleRook->setPosition(getGlobalPosition(sf::Vector2i{ variant->castleQDestination + 1, piece->getLocalPos().y }));
								castleKing->setPosition(getGlobalPosition(sf::Vector2i{ variant->castleQDestination, piece->getLocalPos().y }));
							}
							castleRook->setTarget(getGlobalPosition(sf::Vector2i{ position.castlePieces[1 + offset], piece->getLocalPos().y }));
							castleKing->setTarget(getGlobalPosition(piecePos));
						}
					}
				}
			}
			lastMoveStart.setPosition(getGlobalPosition(newPosition.lastMoveStartLocal));
			lastMoveDest.setPosition(getGlobalPosition(newPosition.lastMoveDestLocal));
			checkSprites.clear();
			for (auto& pos : newPosition.checkPositions) {
				sf::Sprite checkSprite(boardTextures->at(2));
				checkSprite.setOrigin(checkSprite.getLocalBounds().getCenter());
				checkSprite.setScale(boardSprite.getScale());
				checkSprite.setPosition(getGlobalPosition(pos));
				checkSprites.push_back(std::move(checkSprite));
			}
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
		if (allPositionsPlayed.find(lastPositionAdded) != allPositionsPlayed.end()) {
			allPositionsPlayed.find(lastPositionAdded)->second--;
			if (allPositionsPlayed.find(lastPositionAdded)->second <= 0) {
				allPositionsPlayed.erase(lastPositionAdded);
			}
		}
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
		for (auto& piece : position.pieceList) {
			if (piece->hasTarget()) {
				piece->setTarget({});
			}
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
		changingPosition = false;
	}
	if (!positionHistoryF.empty() && !forwardMove) {
		Chess::SavePosition& newPosition = positionHistoryF.front();
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
						if (variant->castlingKingPiece.has(piece->id) && variant->castlingRookPieces.has(capture->id)) {
							closeCastle = true;
						}
						capture.reset();
					}
				}
				if (variant->castlingKingPiece.has(piece->id) && !instantMove) {
					int pieceX = piece->getLocalPos().x, offset = piece->isWhite() ? 0 : 2;
					if (!closeCastle) {
						if (dest.x > pieceX + 1) {
							std::shared_ptr<Piece> rook = getPieceFromCurrentPosition({ position.castlePieces[0 + offset], piece->getLocalPos().y });
							if (rook) {
								castleKing = piece;
								castleRook = rook;
								castleRook->setTarget(getGlobalPosition(sf::Vector2i{ variant->castleKDestination - 1, piece->getLocalPos().y }));
								castleKing->setTarget(getGlobalPosition(sf::Vector2i{ variant->castleKDestination, piece->getLocalPos().y }));
							}
						}
						else if (dest.x < pieceX - 1) {
							std::shared_ptr<Piece> rook = getPieceFromCurrentPosition({ position.castlePieces[1 + offset], piece->getLocalPos().y });
							if (rook) {
								castleKing = piece;
								castleRook = rook;
								castleRook->setTarget(getGlobalPosition(sf::Vector2i{ variant->castleQDestination + 1, piece->getLocalPos().y }));
								castleKing->setTarget(getGlobalPosition(sf::Vector2i{ variant->castleQDestination, piece->getLocalPos().y }));
							}
						}
					}
					else {
						if (dest.x > pieceX) {
							std::shared_ptr<Piece> rook = getPieceFromCurrentPosition({ position.castlePieces[0 + offset], piece->getLocalPos().y });
							if (rook) {
								castleKing = piece;
								castleRook = rook;
								castleRook->setTarget(getGlobalPosition(sf::Vector2i{ variant->castleKDestination - 1, piece->getLocalPos().y }));
								castleKing->setTarget(getGlobalPosition(sf::Vector2i{ variant->castleKDestination, piece->getLocalPos().y }));
							}
						}
						else if (dest.x < pieceX) {
							std::shared_ptr<Piece> rook = getPieceFromCurrentPosition({ position.castlePieces[1 + offset], piece->getLocalPos().y });
							if (rook) {
								castleKing = piece;
								castleRook = rook;
								castleRook->setTarget(getGlobalPosition(sf::Vector2i{ variant->castleQDestination + 1, piece->getLocalPos().y }));
								castleKing->setTarget(getGlobalPosition(sf::Vector2i{ variant->castleQDestination, piece->getLocalPos().y }));
							}
						}
					}
				}
				lastMoveStart.setPosition(getGlobalPosition(newPosition.lastMoveStartLocal));
				lastMoveDest.setPosition(getGlobalPosition(newPosition.lastMoveDestLocal));
				checkSprites.clear();
				for (auto& pos : newPosition.checkPositions) {
					sf::Sprite checkSprite(boardTextures->at(2));
					checkSprite.setOrigin(checkSprite.getLocalBounds().getCenter());
					checkSprite.setScale(boardSprite.getScale());
					checkSprite.setPosition(getGlobalPosition(pos));
					checkSprites.push_back(std::move(checkSprite));
				}
				hasCheck = newPosition.hasCheck;
				std::shared_ptr<Piece> capture = getPieceFromCurrentPosition(dest);
				if (!closeCastle) {
					piece->setTarget(getGlobalPosition(dest));
					if (capture) {
						captureSound.play();
						ghostSprite.setTexture(capture->getTexture(), true);
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
				std::cout << "Target Piece For Position Change Was Not Found at Position: " << piecePos.x << ", " << piecePos.y << std::endl;
			}
		}
		else {
			forwardMove = false;
			loadFromPosition(newPosition);
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
		}
	}
}

BoardSettings Board::getBoardSettings() const
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
	return BoardSettings{ variantIndex, variantTypeIndex, pieceSet, boardSet, AI_Time, stockfishEnabled, AI_Only, position.hasChess960, endgamePosition,
		repeatFEN, timeEnabled, offset.x, offset.y, sizeM, getCurrentFEN(), newPositionFEN, playerSideWhite, newPositionWhite,
		whiteT, blackT, timeIncrement, keyBindsEnabled, optionMode, autoFlip, atomicExplosionEffect, isAnimated,
		overridePieceSpeed, instantMove, isPaused, mouseMode, scaleMode, showMilliseconds, moveSpeed, promotionSquareSelectedColor,
		sWhiteT, sBlackT, millisecondsTime, millisecondsCondition,
		whiteUnit, blackUnit, incrUnit, startingWhiteUnit, startingBlackUnit, millisecondsConditionID, sharedTime, cycleSides };
}

const StockfishSettings& Board::getStockfishSettings() const
{
	return stockfish.getSettings();
}

void Board::loadFromBoardArgs(const BoardSettings& bP, sf::Image& boardSheet)
{
	bool changeVariant = variantIndex != bP.variantIndex || variantTypeIndex != bP.variantTypeIndex;
	if (stockfish.isCalculating && (!stockfish.getSettings().usesFixedTime &&
		(changeVariant || (whiteTime != bP.whiteTime) || (blackTime != bP.blackTime) || (timeIncrement != bP.timeIncrement)))) {
		stockfish.stopCalculating();
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
	boardTexture = Chess::loadBoard(boardSheet, boardSet, variant->boardSize);
	white = bP.white;
	newPositionFEN = bP.newPositionFEN;
	variantIndex = bP.variantIndex;
	variantList = bP.variantList;
	std::string fen = getCurrentFEN();
	if (position.hasChess960 != bP.chess960) {
		position.hasChess960 = bP.chess960;
		if (!changeVariant) {
			generateLegalMoves(false);
		}
	}
	if (changeVariant) {
		setVariant(bP.variantIndex);
	}
	else {
		if (variantPieceTextures->find(variant->name) != variantPieceTextures->end()) {
			currentPieceTextures = &variantPieceTextures->find(variant->name)->second;
		}
		else { currentPieceTextures = &pieceTextures->at(pieceSet); }
		setPieceTextures();
	}

	variantTypeIndex = bP.variantTypeIndex;
	if (bP.FEN.has_value() && bP.FEN != fen) {
		loadFromFEN(bP.FEN.value(), false);
	}
	if (!startedStockfish) {
		stockfish.startStockfish();
		stockfish.setVariant(variant->name, position.hasChess960);
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