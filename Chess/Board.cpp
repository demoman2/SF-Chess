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

Board::Board(const BoardSettings& bA, const StockfishSettings& stockfishSettings, sf::Vector2u windowSize, const std::vector<std::map<char, sf::Texture>>& pieceTextures, const std::map<std::string, std::map<char, sf::Texture>>& variantPieceTextures, const sf::Image& boardSheet, const sf::Font& textFont,
	const std::vector<sf::Texture>& boardTextures, const std::vector<sf::SoundBuffer>& soundBuffers, const std::set<std::string>& endgameVariants, bool init) : variantList(bA.variantList), variantIndex(bA.variantIndex), variant(bA.variantList->at(bA.variantIndex).get()), whiteToPlay(true), playerSideWhite(true), holdingDropPiece(false), AI_Only(bA.AI_Only), endgamePosition(bA.endgamePosition), stockfishEnabled(bA.AI),
	calculatingPos(false), mouseSelecting(false), mouseClicked(false), pieceMoving(false), hasCheck(false), isPromoting(false), scaleMode(bA.scaleMouse), mouseMode(bA.followMouse), pieceSet(bA.pieceSet), boardSet(bA.boardSet), isGating(false),
	animationFinished(false), holdingPiece(false), isPaused(bA.isPaused), repeatFEN(bA.repeatFEN), selectedPiece(nullptr), capturePiece(nullptr), castleKing(nullptr), castleRook(nullptr), promotePiece(nullptr), gatingPiece(nullptr), gatingKing(nullptr), gatingRook(nullptr), cancelGating(false),
	boardTexture(Chess::loadBoard(boardSheet, boardSet, bA.variantList->at(bA.variantIndex)->boardSize)), boardSprite(boardTexture), selectedPieceBackground(boardTextures.at(5)), lastMoveStart(boardTextures.at(3)), lastMoveDest(boardTextures.at(3)), kothBackground(boardTextures.at(8)), kothShadow(boardTextures.at(9)), rankBackground(boardTextures.at(10)), rankShadowTop(boardTextures.at(11)), startingOffset({ bA.xOffset, bA.yOffset }),
	dropPieceBackgroundW(boardTextures.at(13)), dropPieceBackgroundB(boardTextures.at(13)), whiteCheckCount(boardTextures.at(12)), blackCheckCount(boardTextures.at(12)), whiteCheckText(textFont, "3", 40U), blackCheckText(textFont, "3", 40U), soundBuffers(&soundBuffers), moveSound(soundBuffers.at(0)), captureSound(soundBuffers.at(1)), gameEndSound(soundBuffers.at(2)), lowTimeSound(soundBuffers.at(4)), captureThreshold(1.0f), halfMoves(0), fullMoves(0), sizeMultiplier(bA.boardScale), stockfish("assets/other/fairy-stockfish-largeboards_x86-64-modern.exe", stockfishSettings),
	whiteChecksLeft(0), blackChecksLeft(0), isAnimated(bA.boardAnimated), boardTextures(&boardTextures), textFont(&textFont), selectedDropPiece(' ', pieceTextures.front().begin()->second), windowSize(windowSize), selectedPos(), lastMoveStartLocal(100, 100), lastMoveDestLocal(100, 100), shouldPostMove(true), ghostSprite(pieceTextures.front().begin()->second),
	playingMove(false), isFlipped(false), autoFlip(bA.autoFlip), optionMode(bA.showOptionChanges), optionChangeText(textFont, "Paused: True", 70), gameEndText(textFont, "Game Over", 70), boardSize(sf::Vector2f{ 1024.0f, 1024.0f }), generatingMoves(false), startedStockfish(false), movesPlayed(0), white(bA.white), newPositionWhite(bA.newPositionWhite), ai(false), changingPosition(false), forwardMove(false), initialMousePos(), initialBoardScale(1.0f),
	timeClock(), timeEnabled(bA.timeEnabled), whiteTime(bA.whiteTime), blackTime(bA.blackTime), startingWhiteTime(bA.startingWhiteTime), startingBlackTime(bA.startingBlackTime), whiteTimeText(textFont, "00:00", 85), blackTimeText(textFont, "00:00", 85), whiteTimeBG(boardTextures.at(16)), blackTimeBG(boardTextures.at(16)), whiteTimeOutline(boardTextures.at(16)), blackTimeOutline(boardTextures.at(16)), timeIncrement(bA.timeIncrement), boardHSV({ 0, 0, 0 }), gameType(Chess::GameType::Classical), instantMove(false), moveSpeed(1.0f), moveSpeedI(1.0f), instantMoveI(false), AI_Time(bA.AI_Time), newPositionFEN(bA.newPositionFEN),
	millisecondsConditionID(bA.millisecondsConditionID), keyBindsEnabled(bA.keybindsEnabled), atomicExplosionEffect(bA.atomicExplosionEffect), overridePieceSpeed(bA.overridePieceSpeed), showMilliseconds(bA.showMilliseconds), millisecondsTime(bA.millisecondsTime), millisecondsCondition(bA.millisecondsCondition), whiteUnit(bA.whiteUnit), blackUnit(bA.blackUnit), incrUnit(bA.incrUnit), startingWhiteUnit(bA.startingWhiteUnit), startingBlackUnit(bA.startingBlackUnit), promotionSquareSelectedColor(bA.promotionSquareColor), promotionSquareColor(sf::Color::White), position(), lastPositionAdded(std::make_pair<std::vector<std::vector<char>>, bool>({}, false)),
	playerTime(bA.blackTime), AITime(bA.whiteTime), startingPlayerTime(bA.startingBlackTime), startingAITime(bA.startingWhiteTime), sharedTime(bA.sharedTime), cycleSides(bA.cycleSides), arrowLeft(false), arrowRight(false), AI_OnlyT(bA.AI_Only), boardSheet(&boardSheet),
	variantTypeIndex(bA.variantTypeIndex), endgameVariants(&endgameVariants), pieceTextures(&pieceTextures), variantPieceTextures(&variantPieceTextures), currentPieceTextures(&pieceTextures.at(bA.pieceSet)), autoNewPosition(false), doDebug(false)
{
	stockfish.startStockfish();
	startedStockfish = true;

	if (doDebug) {
		auto settings = stockfish.getSettings();
		settings.usesFixedTime = true;
		settings.fixedTime = sf::seconds(0.001f);
		stockfish.loadSettings(settings);
		instantMove = true;
		isAnimated = false;
	}
	// Setup
	boardSprite.setTexture(boardTexture, true);
	if (variantPieceTextures.find(variant->name) != variantPieceTextures.end()) {
		currentPieceTextures = &variantPieceTextures.find(variant->name)->second;
	}
	else { currentPieceTextures = &pieceTextures.at(pieceSet); }

	// Vars
	float ScaleX = windowSize.x / (float)boardTexture.getSize().x;
	float ScaleY = windowSize.y / (float)boardTexture.getSize().y;
	boardScale = std::roundf(std::min(ScaleX, ScaleY) * sizeMultiplier * 1000.0f) / 1000.0f; // Round to 0.00
	startingScale = boardScale;
	pieceScale = boardScale * (128.0f / static_cast<float>(pieceTextures.at(pieceSet).begin()->second.getSize().x));
	boardMultiplier = boardScale * 128.0f;
	boardOffset.x = (windowSize.x / 2.0f) - ((boardTexture.getSize().x * boardScale) / 2.0f) + startingOffset.x;
	boardOffset.y = (windowSize.y / 2.0f) - ((boardTexture.getSize().y * boardScale) / 2.0f) + startingOffset.y;
	this->boardSize = { (boardTexture.getSize().x * boardScale), (boardTexture.getSize().y * boardScale) };
	offset += startingOffset;
	if (!stockfishEnabled) { AI_Only = false; }

	// Sprite Setup
	promotePieceSize = { pieceScale * 0.82f, pieceScale * 0.82f };
	boardSprite.setScale({ boardScale, boardScale });
	boardSprite.setOrigin(boardSprite.getLocalBounds().getCenter());
	boardSprite.setPosition({ boardOffset.x + ((boardTexture.getSize().x * boardScale) / 2.0f),  boardOffset.y + ((boardTexture.getSize().y * boardScale) / 2.0f) });
	promotionOverlay.setSize((sf::Vector2f)boardSprite.getTexture().getSize());
	promotionOverlay.setOrigin(promotionOverlay.getGlobalBounds().getCenter());
	promotionOverlay.setScale(boardSprite.getScale());
	promotionOverlay.setFillColor(sf::Color(35, 35, 35, 150));
	optionChangeOverlay.setSize((sf::Vector2f)boardSprite.getTexture().getSize());
	optionChangeOverlay.setOrigin(optionChangeOverlay.getGlobalBounds().getCenter());
	optionChangeOverlay.setScale(boardSprite.getScale());
	optionChangeOverlay.setFillColor(sf::Color(0, 0, 0, 0));
	optionChangeText.setPosition(boardSprite.getPosition());
	optionChangeText.setOrigin({ optionChangeText.getLocalBounds().position.x + (optionChangeText.getLocalBounds().size.x / 2.0f), optionChangeText.getLocalBounds().position.y + (optionChangeText.getLocalBounds().size.y / 2.0f) });
	optionChangeText.setScale(boardSprite.getScale());
	optionChangeText.setFillColor(sf::Color(0, 0, 0, 0));
	gameEndOverlay.setSize((sf::Vector2f)boardSprite.getTexture().getSize());
	gameEndOverlay.setOrigin(gameEndOverlay.getGlobalBounds().getCenter());
	gameEndOverlay.setScale(boardSprite.getScale());
	gameEndOverlay.setFillColor(sf::Color(0, 0, 0, 0));
	gameEndText.setPosition(boardSprite.getPosition());
	gameEndText.setOrigin({ gameEndText.getLocalBounds().position.x + (gameEndText.getLocalBounds().size.x / 2.0f), gameEndText.getLocalBounds().position.y + (gameEndText.getLocalBounds().size.y / 2.0f) });
	gameEndText.setScale(boardSprite.getScale());
	gameEndText.setFillColor(sf::Color(0, 0, 0, 0));

	selectedPieceBackground.setOrigin(selectedPieceBackground.getLocalBounds().getCenter());
	selectedPieceBackground.setScale(boardSprite.getScale());
	lastMoveDest.setOrigin(lastMoveDest.getLocalBounds().getCenter());
	lastMoveDest.setScale(boardSprite.getScale());
	lastMoveStart.setOrigin(lastMoveStart.getLocalBounds().getCenter());
	lastMoveStart.setScale(boardSprite.getScale());
	kothBackground.setOrigin(kothBackground.getLocalBounds().getCenter());
	kothBackground.setScale(boardSprite.getScale());
	kothShadow.setOrigin(kothShadow.getLocalBounds().getCenter());
	kothShadow.setScale(boardSprite.getScale());
	rankBackground.setOrigin(rankBackground.getLocalBounds().getCenter());
	rankBackground.setScale(boardSprite.getScale());
	rankShadowTop.setOrigin(rankShadowTop.getLocalBounds().getCenter());
	rankShadowTop.setScale(boardSprite.getScale());
	whiteCheckCount.setOrigin(whiteCheckCount.getLocalBounds().getCenter());
	whiteCheckCount.setColor(sf::Color(255, 255, 255, 0));
	whiteCheckCount.setScale({ boardSprite.getScale().x / 3.0f, boardSprite.getScale().y / 3.0f });
	blackCheckCount.setColor(sf::Color(255, 255, 255, 0));
	blackCheckCount.setOrigin(blackCheckCount.getLocalBounds().getCenter());
	blackCheckCount.setScale({ boardSprite.getScale().x / 3.0f, boardSprite.getScale().y / 3.0f });
	whiteCheckText.setOrigin({ whiteCheckText.getLocalBounds().position.x + whiteCheckText.getLocalBounds().size.x / 2.0f, whiteCheckText.getLocalBounds().position.y + whiteCheckText.getLocalBounds().size.y / 2.0f });
	whiteCheckText.setScale({ boardScale, boardScale });
	whiteCheckText.setFillColor(sf::Color(255, 255, 255, 0));
	blackCheckText.setOrigin({ blackCheckText.getLocalBounds().position.x + blackCheckText.getLocalBounds().size.x / 2.0f, blackCheckText.getLocalBounds().position.y + blackCheckText.getLocalBounds().size.y / 2.0f });
	blackCheckText.setScale({ boardScale, boardScale });
	blackCheckText.setFillColor(sf::Color(255, 255, 255, 0));
	ghostSprite.setOrigin(ghostSprite.getLocalBounds().getCenter());
	ghostSprite.setScale(sf::Vector2f(pieceScale, pieceScale));
	ghostSprite.setColor(sf::Color{ 255, 255, 255, 75 });
	kothBackground.setPosition(getGlobalPosition(sf::Vector2f{ variant->boardSize.x / 2.0f + 0.5f, variant->boardSize.y / 2.0f + 0.5f }));
	kothShadow.setPosition(getGlobalPosition(sf::Vector2f{ variant->boardSize.x / 2.0f + 0.5f, variant->boardSize.y / 2.0f + 0.5f }));
	constexpr unsigned int square = (unsigned int)(1024.0f / 8.0f) + 1U;
	boardHSV = RGBtoHSV(boardSprite.getTexture().copyToImage().getPixel({ square, 3 }));

	blackTimeBG.setScale({ boardScale * 1.02f, boardScale * 1.02f });
	blackTimeBG.setPosition({ getGlobalPosition(sf::Vector2f{variant->boardSize.x * 6.5f / 8.0f, variant->boardSize.y + 0.5f}) });
	blackTimeBG.move({ 0, -(blackTimeBG.getTexture().getSize().y * blackTimeBG.getScale().y / 2.0f) - 1 });
	blackTimeBG.setOrigin({ blackTimeBG.getLocalBounds().size.x / 2.0f, blackTimeBG.getLocalBounds().size.y / 2.0f });
	blackTimeBG.setColor(setHueSat(sf::Color(16, 9, 23), boardHSV));
	blackTimeBG.setRotation(sf::degrees(180));
	blackTimeOutline.setScale({ boardScale * 1.11f, boardScale * 1.129f });
	blackTimeOutline.setPosition({ getGlobalPosition(sf::Vector2f{variant->boardSize.x * 6.5f / 8.0f, variant->boardSize.y + 0.5f}) });
	blackTimeOutline.move({ 0, -(blackTimeOutline.getTexture().getSize().y * blackTimeOutline.getScale().y / 2.0f) - 1 });
	blackTimeOutline.setOrigin({ blackTimeOutline.getLocalBounds().size.x / 2.0f, blackTimeOutline.getLocalBounds().size.y / 2.0f });
	blackTimeOutline.setColor(setHueSat(sf::Color(74, 67, 78), boardHSV));
	blackTimeOutline.setRotation(sf::degrees(180));
	blackTimeText.setLetterSpacing(2);
	blackTimeText.setPosition({ blackTimeBG.getPosition() });
	blackTimeText.setOrigin({ blackTimeText.getLocalBounds().position.x + (blackTimeText.getLocalBounds().size.x / 2.0f), blackTimeText.getLocalBounds().position.y + (blackTimeText.getLocalBounds().size.y / 2.0f) });
	blackTimeText.setScale({ boardScale, boardScale });
	blackTimeText.setFillColor(sf::Color(255, 255, 255));

	whiteTimeBG.setScale({ boardScale * 1.02f, boardScale * 1.02f });
	whiteTimeBG.setPosition({ getGlobalPosition(sf::Vector2f{variant->boardSize.x * 2.5f / 8.0f, variant->boardSize.y + 0.5f}) });
	whiteTimeBG.move({ 0, -(whiteTimeBG.getTexture().getSize().y * whiteTimeBG.getScale().y / 2.0f) - 1 });
	whiteTimeBG.setOrigin({ whiteTimeBG.getLocalBounds().size.x / 2.0f, whiteTimeBG.getLocalBounds().size.y / 2.0f });
	whiteTimeBG.setColor(setHueSat(sf::Color(235, 230, 239), boardHSV));
	whiteTimeBG.setRotation(sf::degrees(180));
	whiteTimeOutline.setScale({ boardScale * 1.11f, boardScale * 1.129f });
	whiteTimeOutline.setPosition({ getGlobalPosition(sf::Vector2f{variant->boardSize.x * 2.5f / 8.0f, variant->boardSize.y + 0.5f}) });
	whiteTimeOutline.move({ 0, -(whiteTimeOutline.getTexture().getSize().y * whiteTimeOutline.getScale().y / 2.0f) - 1 });
	whiteTimeOutline.setOrigin({ whiteTimeOutline.getLocalBounds().size.x / 2.0f, whiteTimeOutline.getLocalBounds().size.y / 2.0f });
	whiteTimeOutline.setColor(setHueSat(sf::Color(176, 158, 191), boardHSV));
	whiteTimeOutline.setRotation(sf::degrees(180));
	whiteTimeText.setLetterSpacing(2);
	whiteTimeText.setPosition({ whiteTimeBG.getPosition() });
	whiteTimeText.setOrigin({ whiteTimeText.getLocalBounds().position.x + (whiteTimeText.getLocalBounds().size.x / 2.0f), whiteTimeText.getLocalBounds().position.y + (whiteTimeText.getLocalBounds().size.y / 2.0f) });
	whiteTimeText.setScale({ boardScale, boardScale });
	whiteTimeText.setFillColor(setHueSat(sf::Color(22, 8, 33), boardHSV));

	std::string minutes = std::to_string((int)std::floor(whiteTime.asSeconds() / 60.0f));
	std::string seconds = std::to_string((int)std::fmodf(whiteTime.asSeconds(), 60.0f));
	if (minutes.size() == 1) { minutes.insert(0, "0"); }
	if (seconds.size() == 1) { seconds.insert(0, "0"); }
	whiteTimeText.setString(minutes + ":" + seconds);

	minutes = std::to_string((int)std::floor(blackTime.asSeconds() / 60.0f));
	seconds = std::to_string((int)std::fmodf(blackTime.asSeconds(), 60.0f));
	if (minutes.size() == 1) { minutes.insert(0, "0"); }
	if (seconds.size() == 1) { seconds.insert(0, "0"); }
	blackTimeText.setString(minutes + ":" + seconds);
	setSpritePositions();

	if (timeEnabled) {
		float scaleY = (float)boardTexture.getSize().y / ((float)boardTexture.getSize().y + whiteTimeOutline.getTexture().getSize().y * 1.129f);
		startingScale *= scaleY;
		scale(scaleY);
		moveBy(0, int(whiteTimeOutline.getTexture().getSize().y) * 1.129f / 2.0f);
	}

	// Audio
	moveSound.setVolume(10);
	captureSound.setVolume(15);
	gameEndSound.setVolume(25);
	lowTimeSound.setVolume(10);

	// Stockfish
	delayClock.start();
	if (stockfishEnabled) {
		stockfish.startStockfish();
		startedStockfish = true;
	}

	// FEN Setup
	loadFromFEN(bA.FEN, init, true);

	animationClock.restart();
}

Board::~Board()
{
}